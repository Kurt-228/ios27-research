// Target: AppleM2ScalerCSCDriver — probe v8 (true kr matrix + valid async)
// v7 finding: osz >= 0x2000 makes EVERY call return 0xe00002bf (async-accept
// branch). v8: sync matrix with SMALL outsz (0 and 0x298) to get true per-method
// return codes and verify static sizes; then valid-content async attempts
// (sel 9 with two real surfaces, sel 1 with 0x1b0) and out-buffer post-read.
#include "fuzz.h"
#include <IOSurface/IOSurfaceRef.h>
#include <stdarg.h>

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
        LOG("[rx 0x%x] MSG id 0x%x size %u: %08x %08x %08x %08x %08x %08x %08x %08x",
            port, msg.h.msgh_id, msg.h.msgh_size,
            d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13]);
    }
    return NULL;
}

static IOSurfaceRef make_surface(void) {
    int w = 64, h = 64, bpe = 4, fmt = 0x42475241;
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

// static method map (selector == table index, PROVED from kext disasm):
// sel0 stub, sel1 0x1b0 submit, sel2/3 stubs, sel4 0x20, sel5 getter-u32,
// sel6 0xfa8 batch, sel7 8, sel8 8, sel9 0x10 (two ids), sel10 0x18 (enum<4), sel11 getter 0x298
static const uint32_t meth_sizes[12] = { 0, 0x1b0, 0, 0, 0x20, 0, 0xfa8, 8, 8, 0x10, 0x18, 0 };

static void craft_sel(uint8_t *r, uint32_t sel, size_t sz, IOSurfaceID s1, IOSurfaceID s2) {
    memset(r, 0, 0x1000);
    switch (sel) {
    case 1:  // 0x1b0: +0x08 path(0), +0x20 flags, +0x28..+0x40 fp dims, +0x50/58 ids, +0x68 dims
        *(uint64_t *)(r + 0x08) = 0;
        *(uint64_t *)(r + 0x20) = 0;                       // safe flags first
        *(uint64_t *)(r + 0x28) = 0x0000004000000000ULL;   // 64.0 in 48.16
        *(uint64_t *)(r + 0x30) = 0x0000004000000000ULL;
        *(uint64_t *)(r + 0x38) = 0x0000004000000000ULL;
        *(uint64_t *)(r + 0x40) = 0x0000004000000000ULL;
        *(uint64_t *)(r + 0x50) = s1;
        *(uint64_t *)(r + 0x58) = s2;
        *(uint32_t *)(r + 0x60) = 0x3f800000;              // 1.0f
        *(uint32_t *)(r + 0x64) = 0x3f800000;
        *(uint32_t *)(r + 0x68) = 64;
        *(uint32_t *)(r + 0x6c) = 64;
        break;
    case 6:  // 0xfa8: count<=0x3e8, enable byte +0xfa4
        *(uint32_t *)r = 1;
        r[0xfa4] = 1;
        break;
    case 7: case 8:
        *(uint64_t *)r = s1;
        break;
    case 9:
        *(uint64_t *)r = s1;
        *(uint64_t *)(r + 8) = s2;
        break;
    case 10:
        *(uint32_t *)r = 0;
        break;
    case 4:
        break;
    }
}

static void dump_out(const char *tag, const uint8_t *out, size_t n) {
    // print first 32 bytes if anything differs from 0xAA fill
    int changed = 0;
    for (size_t i = 0; i < n && i < 0x400; i++) if (out[i] != 0xAA) { changed = 1; break; }
    if (changed)
        LOG("%s out: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
            tag, out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7],
            out[8], out[9], out[10], out[11], out[12], out[13], out[14], out[15]);
}

static void probe_v8(io_connect_t conn, IOSurfaceID s1, IOSurfaceID s2) {
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_wake);
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_notify);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, msg_listener, (void *)(uintptr_t)g_wake);
    pthread_create(&t2, NULL, msg_listener, (void *)(uintptr_t)g_notify);
    kern_return_t kr = IOConnectSetNotificationPort(conn, 0, g_notify, 0);
    LOG("[probe8] SetNotificationPort -> 0x%08x", kr);

    uint8_t *req = must_map(0x1000);
    uint8_t *out = must_map(0x2000);

    // PHASE 1: true kr matrix — sel 0..11 x {exact size, 0, wrong size} x outsz {0, 0x298}
    LOG("[probe8] P1 sync matrix sel 0..11, outsz {0, 0x298}");
    static const uint32_t sz_variants[3] = { 0, 1, 2 };  // 0=exact,1=zero-size,2=wrong(0x40)
    for (uint32_t sel = 0; sel <= 11; sel++) {
        for (int v = 0; v < 3; v++) {
            size_t sz = (v == 0) ? meth_sizes[sel] : (v == 1) ? 0 : 0x40;
            for (int ov = 0; ov < 2; ov++) {
                size_t osz = ov ? 0x298 : 0;
                craft_sel(req, sel, sz, s1, s2);
                memset(out, 0xAA, 0x2000);
                uint64_t osc[2] = {0,0}; uint32_t nosc = 0;
                kr = IOConnectCallMethod(conn, sel, NULL, 0, sz ? req : NULL, sz,
                                         osc, &nosc, osz ? out : NULL, &osz);
                LOG("[p1] sel %2u sz 0x%-4zx outsz 0x%-4zx -> kr 0x%08x nosc %u osz %zu",
                    sel, sz, ov ? 0x298 : 0, kr, nosc, osz);
                if (kr == 0 && osz) dump_out("[p1]", out, osz);
            }
        }
    }

    // PHASE 2: valid async — sel 9 (two real ids), sel 1, sel 6, sel 7/8
    LOG("[probe8] P2 valid async attempts");
    static const uint32_t sels_p2[] = { 1, 6, 7, 8, 9, 10 };
    for (unsigned i = 0; i < sizeof(sels_p2)/4; i++) {
        uint32_t sel = sels_p2[i];
        size_t sz = meth_sizes[sel];
        craft_sel(req, sel, sz, s1, s2);
        memset(out, 0xAA, 0x2000);
        size_t osz = 0x2000;
        uint64_t refs[1] = { 0x600000 + sel };
        uint64_t osc[2] = {0,0}; uint32_t nosc = 0;
        kr = IOConnectCallAsyncMethod(conn, sel, g_wake, refs, 1,
                                      NULL, 0, req, sz, osc, &nosc, out, &osz);
        LOG("[p2] async sel %2u (valid) -> kr 0x%08x osz %zu", sel, kr, osz);
        usleep(200000);
        dump_out("[p2] post 200ms", out, 64);
    }
    LOG("[probe8] P2 done, waiting 6s for completions");
    usleep(6000000);
    LOG("[probe8] end");
}

void *t_iosurface_scaler(void *arg) {
    IOSurfaceRef surf1 = make_surface();
    IOSurfaceRef surf2 = make_surface();
    IOSurfaceID s1 = surf1 ? IOSurfaceGetID(surf1) : 0;
    IOSurfaceID s2 = surf2 ? IOSurfaceGetID(surf2) : 0;

    const char *names[] = {
        "AppleM2ScalerCSCDriver", "AppleM2Scaler", "AppleM2ScalerCSCHal",
        "IOSurfaceScaler", "scaler", NULL
    };
    io_connect_t conn = 0;
    for (int i = 0; names[i] && !conn; i++)
        conn = open_service(names[i], 0);
    if (!conn) { LOG("[scaler] not openable"); return NULL; }
    LOG("[scaler] conn 0x%x, s1 %u s2 %u", conn, s1, s2);

    static int probed = 0;
    if (!probed) { probed = 1; probe_v8(conn, s1, s2); }

    // throttled semantic fuzz on real methods with exact sizes
    uint8_t *req = must_map(0x1000);
    uint8_t *out = must_map(0x2000);
    static const uint32_t live[] = { 1, 4, 6, 7, 8, 9, 10 };
    for (long round = 0;; round++) {
        uint32_t sel = live[frand() % (sizeof(live)/4)];
        size_t sz = meth_sizes[sel];
        craft_sel(req, sel, sz, s1, s2);
        if ((frand() & 3) == 0) {  // 25% mutate flags/fields semi-randomly
            if (sel == 1) *(uint64_t *)(req + 0x20) = frand();
            if (sel == 6) { *(uint32_t *)req = frand() % 0x500; req[0xfa4] = frand() & 1; }
            if (sel == 10) *(uint32_t *)req = frand() % 6;
        }
        memset(out, 0xAA, 0x2000);
        size_t osz = (frand() & 1) ? 0x298 : 0x2000;
        uint64_t osc[2] = {0,0}; uint32_t nosc = 0;
        kern_return_t kr;
        if (frand() & 1) {
            uint64_t refs[1] = { frand() };
            kr = IOConnectCallAsyncMethod(conn, sel, g_wake, refs, 1,
                                          NULL, 0, req, sz, osc, &nosc, out, &osz);
        } else {
            kr = IOConnectCallMethod(conn, sel, NULL, 0, req, sz, osc, &nosc, out, &osz);
        }
        if (kr == 0 || ((kr & 0xffff) != 0x2c2 && (kr & 0xffff) != 0x2c7 && (kr & 0xffff) != 0x2bc && (kr & 0xffff) != 0x2bf && (round & 0x3f) == 0))
            LOG("[fuzz] r%ld sel %u kr 0x%08x osz %zu", round, sel, kr, osz);
        if (kr == 0 && osz) dump_out("[fuzz]", out, osz);
        usleep(200 + (frand() & 0x7f));
    }
    return NULL;
}
