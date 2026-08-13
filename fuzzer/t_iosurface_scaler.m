// Target: AppleM2ScalerCSCDriver — v9 (panic bisect + controlled trigger)
// We have a REPRODUCIBLE dart-scaler panic: PTE invalid on write of
// DVA 0x1000003c000. v9 finds the culprit mutation. Every call is logged
// BEFORE issuing, so the last line before panic = trigger.
// Modes (FUZZ_MODE env): flagscan | dimsweep | race | (default) fuzz
#include "fuzz.h"
#include <IOSurface/IOSurfaceRef.h>
#include <stdarg.h>

static IOSurfaceRef make_surface_sz(int w, int h) {
    int bpe = 4, fmt = 0x42475241;
    CFNumberRef W = CFNumberCreate(NULL, kCFNumberIntType, &w);
    CFNumberRef H = CFNumberCreate(NULL, kCFNumberIntType, &h);
    CFNumberRef B = CFNumberCreate(NULL, kCFNumberIntType, &bpe);
    CFNumberRef F = CFNumberCreate(NULL, kCFNumberIntType, &fmt);
    const void *keys[] = { kIOSurfaceWidth, kIOSurfaceHeight, kIOSurfaceBytesPerElement, kIOSurfacePixelFormat };
    const void *vals[] = { W, H, B, F };
    CFDictionaryRef props = CFDictionaryCreate(NULL, keys, vals, 4,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    IOSurfaceRef s = IOSurfaceCreate(props);
    CFRelease(props); CFRelease(W); CFRelease(H); CFRelease(B); CFRelease(F);
    return s;
}

// valid sel-1 request (0x1b0), from static field map
static void craft_sel1(uint8_t *r, IOSurfaceID s1, IOSurfaceID s2, uint64_t flags) {
    memset(r, 0, 0x1b0);
    *(uint64_t *)(r + 0x08) = 0;                          // path B (scheduler)
    *(uint64_t *)(r + 0x20) = flags;
    *(uint64_t *)(r + 0x28) = 0x0000004000000000ULL;      // 64.0 fp(48.16)
    *(uint64_t *)(r + 0x30) = 0x0000004000000000ULL;
    *(uint64_t *)(r + 0x38) = 0x0000004000000000ULL;
    *(uint64_t *)(r + 0x40) = 0x0000004000000000ULL;
    *(uint64_t *)(r + 0x50) = s1;
    *(uint64_t *)(r + 0x58) = s2;
    *(uint32_t *)(r + 0x60) = 0x3f800000;                 // 1.0f
    *(uint32_t *)(r + 0x64) = 0x3f800000;
    *(uint32_t *)(r + 0x68) = 64;
    *(uint32_t *)(r + 0x6c) = 64;
}

static kern_return_t call1(io_connect_t conn, uint8_t *req, uint8_t *out, size_t osz) {
    uint64_t osc[2] = {0,0}; uint32_t nosc = 0;
    return IOConnectCallMethod(conn, 1, NULL, 0, req, 0x1b0, osc, &nosc, out, &osz);
}

void *t_iosurface_scaler(void *arg) {
    const char *mode = getenv("FUZZ_MODE");
    IOSurfaceRef surf1 = make_surface_sz(64, 64);
    IOSurfaceRef surf2 = make_surface_sz(64, 64);
    IOSurfaceID s1 = surf1 ? IOSurfaceGetID(surf1) : 0;
    IOSurfaceID s2 = surf2 ? IOSurfaceGetID(surf2) : 0;

    const char *names[] = { "AppleM2ScalerCSCDriver", "AppleM2Scaler", "AppleM2ScalerCSCHal", "IOSurfaceScaler", "scaler", NULL };
    io_connect_t conn = 0;
    for (int i = 0; names[i] && !conn; i++)
        conn = open_service(names[i], 0);
    if (!conn) { LOG("[scaler] not openable"); return NULL; }
    LOG("[v9] mode=%s conn 0x%x s1 %u s2 %u", mode ? mode : "fuzz", conn, s1, s2);

    uint8_t *req = must_map(0x1000);
    uint8_t *out = must_map(0x2000);

    if (mode && !strcmp(mode, "flagscan")) {
        // one flag bit at a time, valid surfaces — which bit drops the DART?
        for (int bit = 0; bit < 64; bit++) {
            uint64_t flags = 1ULL << bit;
            craft_sel1(req, s1, s2, flags);
            LOG("[flagscan] bit %d flags %016llx ->", bit, (unsigned long long)flags);
            kern_return_t kr = call1(conn, req, out, 0x298);
            LOG("[flagscan] bit %d <- kr 0x%08x", bit, kr);
            usleep(50000);
        }
        LOG("[flagscan] done, no panic");
        return NULL;
    }

    if (mode && !strcmp(mode, "dimsweep")) {
        // geometry mutations: fp dims and u32 dims around/past buffer size
        static const uint64_t fp_vals[] = {
            0x0000004000000000ULL,  // 64
            0x0000004100000000ULL,  // 65 (just past)
            0x0000008000000000ULL,  // 128
            0x0000040000000000ULL,  // 1024
            0x0000FFFFFFFFFFFFULL,  // huge
            0x0000004080000000ULL,  // 64.5 (fractional — rounding check)
            0x1, 0x0,
        };
        static const uint32_t u32_vals[] = { 64, 65, 128, 1024, 4096, 0xffffffff, 1, 0 };
        int it = 0;
        for (unsigned a = 0; a < sizeof(fp_vals)/8; a++) {
            for (unsigned b = 0; b < sizeof(u32_vals)/4; b++) {
                craft_sel1(req, s1, s2, 0);
                *(uint64_t *)(req + 0x28) = fp_vals[a];
                *(uint64_t *)(req + 0x30) = fp_vals[a];
                *(uint64_t *)(req + 0x38) = fp_vals[a];
                *(uint64_t *)(req + 0x40) = fp_vals[a];
                *(uint32_t *)(req + 0x68) = u32_vals[b];
                *(uint32_t *)(req + 0x6c) = u32_vals[b];
                LOG("[dimsweep] it %d fp %016llx u32 %08x ->", it, (unsigned long long)fp_vals[a], u32_vals[b]);
                kern_return_t kr = call1(conn, req, out, 0x298);
                LOG("[dimsweep] it %d <- kr 0x%08x", it, kr);
                it++;
                usleep(50000);
            }
        }
        LOG("[dimsweep] done, no panic");
        return NULL;
    }

    if (mode && !strcmp(mode, "race")) {
        // free surfaces while DMA in flight; hope for stale-PTE write into reused page
        __block volatile int stop = 0;
        dispatch_queue_t q = dispatch_queue_create("churn", DISPATCH_QUEUE_CONCURRENT);
        dispatch_async(q, ^{
            while (!stop) {
                IOSurfaceRef t = make_surface_sz(64, 64);
                if (t) CFRelease(t);
            }
        });
        for (long round = 0;; round++) {
            craft_sel1(req, s1, s2, frand());   // random flags to vary DMA shape
            if ((round & 0xff) == 0)
                LOG("[race] r%ld ->", round);
            kern_return_t kr = call1(conn, req, out, 0x298);
            if ((round & 0xff) == 0)
                LOG("[race] r%ld <- kr 0x%08x", round, kr);
            if ((round & 0x3ff) == 0 && surf1) {
                CFRelease(surf1); CFRelease(surf2);
                surf1 = make_surface_sz(64, 64); surf2 = make_surface_sz(64, 64);
                s1 = surf1 ? IOSurfaceGetID(surf1) : 0;
                s2 = surf2 ? IOSurfaceGetID(surf2) : 0;
            }
            usleep(200 + (frand() & 0x7f));
        }
        return NULL;
    }

    // default: replay of v8 fuzz but with pre-call logging of key fields
    static const uint32_t meth_sizes[12] = { 0, 0x1b0, 0, 0, 0x20, 0, 0xfa8, 8, 8, 0x10, 0x18, 0 };
    static const uint32_t live[] = { 1, 4, 6, 7, 8, 9, 10 };
    for (long round = 0;; round++) {
        uint32_t sel = live[frand() % (sizeof(live)/4)];
        size_t sz = meth_sizes[sel];
        memset(req, 0, 0x1000);
        if (sel == 1) craft_sel1(req, s1, s2, frand());
        else if (sel == 6) { *(uint32_t *)req = frand() % 0x500; req[0xfa4] = frand() & 1; }
        else if (sel == 7 || sel == 8) *(uint64_t *)req = s1;
        else if (sel == 9) { *(uint64_t *)req = s1; *(uint64_t *)(req+8) = s2; }
        else if (sel == 10) *(uint32_t *)req = frand() % 6;
        memset(out, 0xAA, 0x2000);
        size_t osz = 0x298;
        uint64_t osc[2] = {0,0}; uint32_t nosc = 0;
        LOG("[fuzz] r%ld sel %u flags/head %016llx ->", round, sel, *(unsigned long long *)(req + 0x20));
        kern_return_t kr = IOConnectCallMethod(conn, sel, NULL, 0, sz ? req : NULL, sz,
                                               osc, &nosc, out, &osz);
        if (kr != 0xe00002c2 && kr != 0xe00002c7)
            LOG("[fuzz] r%ld sel %u <- kr 0x%08x osz %zu", round, sel, kr, osz);
        usleep(200 + (frand() & 0x7f));
        if ((round & 0x3fff) == 0 && surf1) {
            CFRelease(surf1); CFRelease(surf2);
            surf1 = make_surface_sz(64, 64); surf2 = make_surface_sz(64, 64);
            s1 = surf1 ? IOSurfaceGetID(surf1) : 0;
            s2 = surf2 ? IOSurfaceGetID(surf2) : 0;
        }
    }
    return NULL;
}
