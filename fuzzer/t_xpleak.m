// Target: cross-process GPU memory leak (journal v90/v91 follow-up).
// Hypothesis: GPU pages of a SIGKILLed process are re-assigned to new owners
// WITHOUT scrubbing (seen via GPU read-probes inside one harness). This file
// is a clean two-role experiment driven by FUZZ_XPLEAK:
//   FUZZ_XPLEAK=victim  allocate MTLBuffer(storageModeShared)/IOSurface bulk,
//                       fill each with a unique marker (0xCAFEB0BA sig +
//                       buffer index + pointer-like qwords 0x00000009_xxxxxxxx
//                       + secret-looking strings), hold, log stats, then
//                       kill(getpid(), SIGKILL) WITHOUT freeing (jetsam/crash
//                       simulation; a normal exit may free cleanly).
//   FUZZ_XPLEAK=obs     (default, also FUZZ_XPLEAK=1) aggressive waves of
//                       allocations of the same classes, CPU-readback right
//                       after each alloc, scan for: (a) any non-zero bytes,
//                       (b) victim signature, (c) pointer-like qwords
//                       (0x09_xxxxxxxx heap VA = foreign service structs).
//                       First obs run WITHOUT a preceding victim run = the
//                       "background dirt" control baseline.
//   FUZZ_XPLEAK=loop    victim-style fill, then free everything WITHOUT
//                       killing and immediately scan new allocations
//                       (same-process free-reuse control, separates plain
//                       userland reuse from the cross-process path).
// Knobs:
//   FUZZ_XPLEAK_KIND=mtl|iosurf|both   (default both)
//   FUZZ_XPLEAK_SECS=<n>               obs/loop scan cap, default 240
//   FUZZ_XPLEAK_MB=<n>                 victim fill target in MB, default 256
//   FUZZ_XPLEAK_SKIP_WAVES=<n>         resume: skip first n obs waves entirely
//                                      (if wave n crashed the process)
//   FUZZ_XPLEAK_NOKILL=1               victim debug: skip the SIGKILL
#include "fuzz.h"
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <IOSurface/IOSurfaceRef.h>
#include <signal.h>
#include <time.h>

#define XP_SIG32   0xCAFEB0BAu
#define XP_MAGIC2  0xDEADBEEFCAFEF00DULL
#define XP_PTR_HI  0x0000000900000000ULL   // pointer-like heap VA prefix
#define XP_PTR_MASK 0xFFFFFF0000000000ULL

static double xp_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static int xp_kind_mtl(void) {
    const char *k = getenv("FUZZ_XPLEAK_KIND");
    return !k || !strcmp(k, "both") || !strcmp(k, "mtl");
}
static int xp_kind_iosurf(void) {
    const char *k = getenv("FUZZ_XPLEAK_KIND");
    return !k || !strcmp(k, "both") || !strcmp(k, "iosurf");
}

// ---- marker -------------------------------------------------------------
// Layout: [0x00] sig qword 0x00000000CAFEB0BA | [0x08] buffer index |
//         [0x10] MAGIC2 ^ idx | [0x18] epoch | rest: pointer-like qwords
//         interleaved with secret-looking ASCII.
static void xp_fill_marker(uint8_t *p, size_t n, uint32_t idx) {
    if (n < 0x40) return;
    uint64_t hdr[4] = { XP_SIG32, (uint64_t)idx,
                        XP_MAGIC2 ^ idx, (uint64_t)time(NULL) };
    memcpy(p, hdr, sizeof(hdr));
    for (size_t off = 0x40; off + 16 <= n; off += 16) {
        uint64_t v = XP_PTR_HI | ((uint64_t)(idx & 0xffff) << 16) |
                     (uint64_t)(off & 0xfff0);
        memcpy(p + off, &v, 8);
        if ((off & 0x1ff) == 0x40) {
            static const char hexd[] = "0123456789abcdef";
            uint8_t *s = p + off + 8;   // exactly 8 bytes, no NUL (no overflow)
            s[0] = 'S'; s[1] = '3'; s[2] = 'C'; s[3] = 'R';
            s[4] = '3'; s[5] = 'T';
            s[6] = (uint8_t)hexd[(idx >> 4) & 0xf];
            s[7] = (uint8_t)hexd[idx & 0xf];
        } else
            memset(p + off + 8, 0x41 + (idx & 0x1f), 8);
    }
}

// ---- scan ---------------------------------------------------------------
struct xp_scan_res {
    uint64_t nz_bytes;      // any non-zero residue
    uint64_t sig_hits;      // victim signature occurrences
    uint64_t sig_off[8];    // first sig offsets
    uint64_t ptr_hits;      // pointer-like qwords 0x09_xxxxxxxx
    uint64_t ptr_first_off;
    uint64_t ptr_first_val;
};

static void xp_scan(const uint8_t *p, size_t n, struct xp_scan_res *r) {
    memset(r, 0, sizeof(*r));
    r->ptr_first_off = ~0ULL;
    // qword pass: non-zero + pointer-like
    size_t nq = n / 8;
    for (size_t i = 0; i < nq; i++) {
        uint64_t v;
        memcpy(&v, p + i * 8, 8);
        if (v) r->nz_bytes += 8;   // upper bound; refined below if needed
        if ((v & XP_PTR_MASK) == XP_PTR_HI) {
            if (r->ptr_first_off == ~0ULL) {
                r->ptr_first_off = i * 8;
                r->ptr_first_val = v;
            }
            r->ptr_hits++;
        }
    }
    for (size_t i = nq * 8; i < n; i++)
        if (p[i]) r->nz_bytes++;
    // signature pass via memchr on 0xBA (LE bytes of 0xCAFEB0BA: BA B0 FE CA)
    size_t off = 0;
    while (off + 4 <= n) {
        const uint8_t *hit = memchr(p + off, 0xBA, n - off - 3);
        if (!hit) break;
        size_t ho = (size_t)(hit - p);
        if (hit[1] == 0xB0 && hit[2] == 0xFE && hit[3] == 0xCA) {
            if (r->sig_hits < 8) r->sig_off[r->sig_hits] = ho;
            r->sig_hits++;
        }
        off = ho + 1;
    }
}

static void xp_hexdump64(const char *tag, const uint8_t *p) {
    for (int i = 0; i < 64; i += 16)
        LOG("%s +%02x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
            tag, i, p[i], p[i+1], p[i+2], p[i+3], p[i+4], p[i+5], p[i+6], p[i+7],
            p[i+8], p[i+9], p[i+10], p[i+11], p[i+12], p[i+13], p[i+14], p[i+15]);
}

// ---- IOSurface helpers ----------------------------------------------------
static IOSurfaceRef xp_make_surface(int w, int h, int bpe, int fmt) {
    CFNumberRef W = CFNumberCreate(NULL, kCFNumberIntType, &w);
    CFNumberRef H = CFNumberCreate(NULL, kCFNumberIntType, &h);
    CFNumberRef B = CFNumberCreate(NULL, kCFNumberIntType, &bpe);
    CFNumberRef F = CFNumberCreate(NULL, kCFNumberIntType, &fmt);
    const void *keys[] = { kIOSurfaceWidth, kIOSurfaceHeight,
                           kIOSurfaceBytesPerElement, kIOSurfacePixelFormat };
    const void *vals[] = { W, H, B, F };
    CFDictionaryRef props = CFDictionaryCreate(NULL, keys, vals, 4,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    IOSurfaceRef s = IOSurfaceCreate(props);
    CFRelease(props); CFRelease(W); CFRelease(H); CFRelease(B); CFRelease(F);
    return s;
}

// plane-0 span; safe for BGRA and 420v (biplanar: base = plane 0)
static size_t xp_surface_span(IOSurfaceRef s) {
    return IOSurfaceGetBytesPerRow(s) * IOSurfaceGetHeight(s);
}

// ---- victim ---------------------------------------------------------------
static const size_t xp_mtl_sizes[] = { 0x4000, 0x8000, 0x10000, 0x20000,
                                       0x40000, 0x80000, 0x100000 };
#define XP_N_MTL_SIZES (sizeof(xp_mtl_sizes)/sizeof(xp_mtl_sizes[0]))
// BGRA (bpe 4) and 420v (bpe 1) geometry table
static const struct { int w, h, bpe; uint32_t fmt; } xp_surf_geo[] = {
    { 64, 64, 4, 0x42475241 },     // 16KB  BGRA
    { 256, 256, 4, 0x42475241 },   // 256KB BGRA
    { 512, 512, 4, 0x42475241 },   // 1MB   BGRA
    { 1024, 1024, 4, 0x42475241 }, // 4MB   BGRA
    { 128, 128, 1, 0x34323076 },   // 420v
    { 640, 480, 1, 0x34323076 },   // 420v VGA
    { 1280, 720, 1, 0x34323076 },  // 420v 720p
    { 1920, 1080, 1, 0x34323076 }, // 420v 1080p
};
#define XP_N_SURF_GEO (sizeof(xp_surf_geo)/sizeof(xp_surf_geo[0]))

// fills up to target_mb of marked buffers; returns arrays via out params.
// Caller decides what to do with them (kill / free+scan).
static void xp_victim_fill(id<MTLDevice> dev, uint64_t target_mb,
                           NSMutableArray *mtl_keep, CFMutableArrayRef sf_keep,
                           uint64_t *out_bytes) {
    uint64_t target = target_mb * 1024 * 1024;
    uint64_t total = 0;
    uint32_t idx = 0;
    int gi = 0;
    LOG("[xp-victim] fill start, target %llu MB, kind mtl=%d iosurf=%d",
        target_mb, xp_kind_mtl(), xp_kind_iosurf());
    while (total < target) {
        int do_mtl = xp_kind_mtl() && (!xp_kind_iosurf() || (idx & 1) == 0);
        if (do_mtl) {
            size_t sz = xp_mtl_sizes[idx % XP_N_MTL_SIZES];
            LOG("[xp-victim] alloc MTLBuffer #%u size 0x%zx", idx, sz);
            id<MTLBuffer> b = [dev newBufferWithLength:sz
                                               options:MTLResourceStorageModeShared];
            if (!b) { LOG("[xp-victim] MTLBuffer alloc FAILED at #%u, stop fill", idx); break; }
            xp_fill_marker((uint8_t *)[b contents], sz, idx);
            [mtl_keep addObject:b];
            total += sz;
        } else {
            int w = xp_surf_geo[gi].w, h = xp_surf_geo[gi].h,
                bpe = xp_surf_geo[gi].bpe;
            uint32_t fmt = xp_surf_geo[gi].fmt;
            gi = (gi + 1) % (int)XP_N_SURF_GEO;
            LOG("[xp-victim] alloc IOSurface #%u %dx%d bpe %d fmt %.4s",
                idx, w, h, bpe, (char *)&fmt);
            IOSurfaceRef s = xp_make_surface(w, h, bpe, (int)fmt);
            if (!s) { LOG("[xp-victim] IOSurfaceCreate FAILED at #%u, stop fill", idx); break; }
            uint32_t seed = 0;
            if (IOSurfaceLock(s, 0, &seed)) {
                LOG("[xp-victim] IOSurfaceLock FAILED at #%u, skip fill", idx);
            } else {
                size_t span = xp_surface_span(s);
                xp_fill_marker((uint8_t *)IOSurfaceGetBaseAddress(s), span, idx);
                IOSurfaceUnlock(s, 0, &seed);
                total += span;
            }
            CFArrayAppendValue(sf_keep, s);
            CFRelease(s);   // array holds the only ref now
        }
        idx++;
    }
    *out_bytes = total;
    LOG("[xp-victim] fill done: %u buffers, %llu bytes (%.1f MB)",
        idx, total, (double)total / 1048576.0);
    // first markers for later correlation in obs logs
    int shown = 0;
    for (id<MTLBuffer> b in mtl_keep) {
        if (shown >= 4) break;
        const uint64_t *q = (const uint64_t *)[b contents];
        LOG("[xp-victim] mtl marker[%d]: %016llx %016llx %016llx %016llx",
            shown, q[0], q[1], q[2], q[3]);
        shown++;
    }
    for (CFIndex i = 0; i < CFArrayGetCount(sf_keep) && shown < 8; i++, shown++) {
        IOSurfaceRef s = (IOSurfaceRef)CFArrayGetValueAtIndex(sf_keep, i);
        uint32_t seed = 0;
        if (IOSurfaceLock(s, kIOSurfaceLockReadOnly, &seed)) continue;
        const uint64_t *q = (const uint64_t *)IOSurfaceGetBaseAddress(s);
        LOG("[xp-victim] surf marker[%d]: %016llx %016llx %016llx %016llx",
            shown, q[0], q[1], q[2], q[3]);
        IOSurfaceUnlock(s, kIOSurfaceLockReadOnly, &seed);
    }
    fflush(stderr);
}

static void xp_run_victim(id<MTLDevice> dev) {
    const char *mbs = getenv("FUZZ_XPLEAK_MB");
    uint64_t mb = mbs ? strtoull(mbs, NULL, 0) : 256;
    NSMutableArray *mtl_keep = [NSMutableArray array];
    CFMutableArrayRef sf_keep = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    uint64_t total = 0;
    xp_victim_fill(dev, mb, mtl_keep, sf_keep, &total);
    // hold briefly so backing pages are definitely instantiated GPU-side
    LOG("[xp-victim] holding %llu bytes for 2s before kill", total);
    fflush(stderr);
    usleep(2000 * 1000);
    if (getenv("FUZZ_XPLEAK_NOKILL")) {
        LOG("[xp-victim] NOKILL set, returning without SIGKILL (debug)");
        CFRelease(sf_keep);
        return;
    }
    LOG("[xp-victim] SIGKILL now (no free) pid %d — next run: FUZZ_XPLEAK=obs",
        getpid());
    fflush(stderr);
    kill(getpid(), SIGKILL);
    LOG("[xp-victim] ERROR: survived SIGKILL?!");
    CFRelease(sf_keep);
}

// ---- observer -------------------------------------------------------------
struct xp_obs_totals {
    uint64_t bufs, nonzero, sig, ptr, bytes;
};

static void xp_obs_scan_buf(const char *cls, int wave, int bi,
                            const uint8_t *p, size_t n,
                            struct xp_obs_totals *T, int *hexdumped) {
    struct xp_scan_res r;
    xp_scan(p, n, &r);
    T->bufs++;
    T->bytes += n;
    if (!r.nz_bytes && !r.sig_hits && !r.ptr_hits) return;
    if (r.nz_bytes)  T->nonzero++;
    if (r.sig_hits)  T->sig++;
    if (r.ptr_hits)  T->ptr++;
    LOG("[xp-obs] w%d %s#%d size 0x%zx: nz %llu sig %llu ptr %llu",
        wave, cls, bi, n, r.nz_bytes, r.sig_hits, r.ptr_hits);
    if (r.sig_hits) {
        LOG("[xp-obs] w%d %s#%d SIG offsets: %llu %llu %llu %llu %llu %llu %llu %llu",
            wave, cls, bi,
            r.sig_off[0], r.sig_off[1], r.sig_off[2], r.sig_off[3],
            r.sig_off[4], r.sig_off[5], r.sig_off[6], r.sig_off[7]);
    }
    if (r.ptr_hits)
        LOG("[xp-obs] w%d %s#%d first ptr-like qword @0x%llx = 0x%016llx",
            wave, cls, bi, r.ptr_first_off, r.ptr_first_val);
    if (*hexdumped < 3) {
        xp_hexdump64("[xp-obs] head", p);
        (*hexdumped)++;
    }
}

static void xp_run_obs(id<MTLDevice> dev, const char *tag) {
    const char *ss = getenv("FUZZ_XPLEAK_SECS");
    double secs = ss ? atof(ss) : 240.0;
    const char *sk = getenv("FUZZ_XPLEAK_SKIP_WAVES");
    int skip = sk ? atoi(sk) : 0;
    struct xp_obs_totals T = {0, 0, 0, 0, 0};
    double t0 = xp_now();
    LOG("[xp-obs] scan start tag=%s, cap %.0fs, skip %d waves, kind mtl=%d iosurf=%d, pid %d",
        tag, secs, skip, xp_kind_mtl(), xp_kind_iosurf(), getpid());
    fflush(stderr);
    for (int w = 0; ; w++) {
        if (xp_now() - t0 > secs) break;
        if (w < skip) { LOG("[xp-obs] skipping wave %d (resume)", w); continue; }
        size_t mtl_sz = xp_mtl_sizes[w % XP_N_MTL_SIZES];
        int geo = w % (int)XP_N_SURF_GEO;
        LOG("[xp-obs] wave %d: mtl 0x%zx x16, surf %dx%d/%d/%.4s x8 (t+%.0fs)",
            w, mtl_sz, xp_surf_geo[geo].w, xp_surf_geo[geo].h,
            xp_surf_geo[geo].bpe, (char *)&xp_surf_geo[geo].fmt, xp_now() - t0);
        fflush(stderr);
        int hexdumped = 0;
        if (xp_kind_mtl()) {
            for (int i = 0; i < 16; i++) {
                id<MTLBuffer> b = [dev newBufferWithLength:mtl_sz
                                                   options:MTLResourceStorageModeShared];
                if (!b) { LOG("[xp-obs] w%d mtl alloc fail #%d", w, i); break; }
                xp_obs_scan_buf("mtl", w, i, (const uint8_t *)[b contents],
                                mtl_sz, &T, &hexdumped);
            }
        }
        if (xp_kind_iosurf()) {
            for (int i = 0; i < 8; i++) {
                IOSurfaceRef s = xp_make_surface(xp_surf_geo[geo].w,
                                                 xp_surf_geo[geo].h,
                                                 xp_surf_geo[geo].bpe,
                                                 (int)xp_surf_geo[geo].fmt);
                if (!s) { LOG("[xp-obs] w%d surf alloc fail #%d", w, i); break; }
                uint32_t seed = 0;
                if (IOSurfaceLock(s, kIOSurfaceLockReadOnly, &seed)) {
                    LOG("[xp-obs] w%d surf#%d rlock fail", w, i);
                } else {
                    xp_obs_scan_buf("surf", w, i,
                                    (const uint8_t *)IOSurfaceGetBaseAddress(s),
                                    xp_surface_span(s), &T, &hexdumped);
                    IOSurfaceUnlock(s, kIOSurfaceLockReadOnly, &seed);
                }
                CFRelease(s);
            }
        }
        LOG("[xp-obs] wave %d done: total bufs %llu nonzero %llu sig %llu ptr %llu (%.1f MB read)",
            w, T.bufs, T.nonzero, T.sig, T.ptr, (double)T.bytes / 1048576.0);
        fflush(stderr);
        usleep(100 * 1000);   // let the pool breathe, jetsam avoid
    }
    LOG("[xp-obs] scan end tag=%s: bufs %llu, nonzero %llu, sig-hit bufs %llu, ptr-hit bufs %llu, %.1f MB read in %.0fs",
        tag, T.bufs, T.nonzero, T.sig, T.ptr,
        (double)T.bytes / 1048576.0, xp_now() - t0);
    fflush(stderr);
}

// ---- loop: same-process free-reuse control ---------------------------------
static void xp_run_loop(id<MTLDevice> dev) {
    const char *mbs = getenv("FUZZ_XPLEAK_MB");
    uint64_t mb = mbs ? strtoull(mbs, NULL, 0) : 256;
    uint64_t total = 0;
    {
        NSMutableArray *mtl_keep = [NSMutableArray array];
        CFMutableArrayRef sf_keep = CFArrayCreateMutable(NULL, 0,
                                                         &kCFTypeArrayCallBacks);
        xp_victim_fill(dev, mb, mtl_keep, sf_keep, &total);
        LOG("[xp-loop] freeing %llu bytes WITHOUT kill, then scanning same-process reuse",
            total);
        fflush(stderr);
        [mtl_keep removeAllObjects];   // ARC releases the MTLBuffers
        CFRelease(sf_keep);            // releases the IOSurfaces
    }
    LOG("[xp-loop] free done, starting scan");
    fflush(stderr);
    xp_run_obs(dev, "loop");
}

// ---- entry -------------------------------------------------------------------
void *t_xpleak(void *arg) {
    (void)arg;
    const char *mode = getenv("FUZZ_XPLEAK");
    if (!mode) mode = "obs";
    int is_obs = !strcmp(mode, "obs") || !strcmp(mode, "1");
    LOG("[xpleak] mode=%s kind=%s pid %d", mode,
        getenv("FUZZ_XPLEAK_KIND") ? getenv("FUZZ_XPLEAK_KIND") : "both",
        getpid());
    id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
    if (!dev && xp_kind_mtl()) {
        LOG("[xpleak] MTLCreateSystemDefaultDevice failed and kind needs mtl, abort");
        return NULL;
    }
    if (!strcmp(mode, "victim"))      xp_run_victim(dev);
    else if (!strcmp(mode, "loop"))   xp_run_loop(dev);
    else if (is_obs)                  xp_run_obs(dev, "obs");
    else LOG("[xpleak] unknown FUZZ_XPLEAK='%s' (want victim|obs|loop)", mode);
    LOG("[xpleak] mode=%s finished, stop", mode);
    return NULL;
}
