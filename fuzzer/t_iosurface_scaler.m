// Target: AppleM2ScalerCSCDriver — probe v5 (async OSAction invocation)
// Decoded: sel 2,3,12-31 return kIOReturnNoCompletion (0xe00002bf) on sync calls
// with structureOutput >= 0x2000 -> they are ASYNC OSActions. Invoke via
// IOConnectCallAsyncMethod and read completions from the wake port.
#include "fuzz.h"
#include <IOSurface/IOSurfaceRef.h>

#define REQ_SZ 0x1b0
#define OUT_SZ 0x2380

static void craft_request(uint8_t *r, IOSurfaceID sid) {
    fill_semi_structured(r, REQ_SZ);
    if (frand() & 1) {
        *(uint32_t *)(r + 0x68) = (uint32_t)frand_range(1, 4096);
        *(uint32_t *)(r + 0x6c) = (uint32_t)frand_range(1, 4096);
        *(uint32_t *)(r + 0x60) = 0x3f800000;
        *(uint32_t *)(r + 0x64) = 0x3f800000;
    }
    if (sid) {
        *(uint64_t *)(r + 0x50) = sid;
        *(uint64_t *)(r + 0x58) = sid;
        *(uint32_t *)(r + 0xd0) = (uint32_t)sid;
        if (frand() & 1) {
            size_t off = (frand_range(0, REQ_SZ - 8)) & ~7ULL;
            *(uint64_t *)(r + off) = sid;
        }
    }
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

static mach_port_t g_wake;

static void *completion_listener(void *arg) {
    struct { mach_msg_header_t h; uint8_t data[0x200]; } msg;
    for (;;) {
        kern_return_t kr = mach_msg(&msg.h, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0,
                                    sizeof(msg), g_wake, 2000, MACH_PORT_NULL);
        if (kr == MACH_RCV_TIMED_OUT) continue;
        if (kr) { LOG("[async-rx] mach_msg err 0x%x", kr); continue; }
        uint32_t *d = (uint32_t *)&msg;
        LOG("[async-rx] id 0x%x size %u: %08x %08x %08x %08x %08x %08x",
            msg.h.msgh_id, msg.h.msgh_size, d[6], d[7], d[8], d[9], d[10], d[11]);
    }
    return NULL;
}

static void async_call(io_connect_t conn, uint32_t sel, const void *in, size_t insz,
                       uint8_t *out, size_t *outsz, uint32_t sc_in, uint32_t sc_out) {
    uint64_t scalars[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint64_t outScalars[8] = {0};
    uint64_t refs[1] = { 0x1234 };
    kern_return_t kr = IOConnectCallAsyncMethod(conn, sel, g_wake, refs, sc_in ? 1 : 0,
        sc_in ? scalars : NULL, sc_in,
        in, insz,
        outScalars, sc_out ? &sc_out : NULL,
        out, outsz);
    if (kr && kr != 0xe00002bf && kr != 0xe00002c2 && kr != 0xe00002c7 &&
        kr != 0xe00002c9 && kr != 0xe00002bc && kr != 0xe00002f0 && kr != 0xe00002e2)
        LOG("[async] sel %u call -> 0x%08x", sel, kr);
}

static void probe_v5(io_connect_t conn, IOSurfaceID sid) {
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_wake);
    pthread_t lt;
    pthread_create(&lt, NULL, completion_listener, NULL);

    uint8_t *req = must_map(REQ_SZ);
    uint8_t *out = must_map(OUT_SZ);
    LOG("[probe5] async sweep sel 2,3,12-31 with refs/scalars variants");
    static const uint32_t sels[] = { 2, 3, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
    for (unsigned si = 0; si < sizeof(sels)/4; si++) {
        uint32_t sel = sels[si];
        for (uint32_t sc = 0; sc <= 2; sc++) {
            craft_request(req, sid);
            memset(out, 0, OUT_SZ);
            size_t osz = OUT_SZ;
            async_call(conn, sel, req, REQ_SZ, out, &osz, sc, 0);
            usleep(5000);
        }
    }
    LOG("[probe5] sweep done; waiting 3s for completions");
    usleep(3000000);
    LOG("[probe5] end");
}

void *t_iosurface_scaler(void *arg) {
    uint8_t *req = must_map(REQ_SZ);
    IOSurfaceRef surf = make_surface();
    IOSurfaceID sid = surf ? IOSurfaceGetID(surf) : 0;

    const char *names[] = {
        "AppleM2ScalerCSCDriver", "AppleM2Scaler", "AppleM2ScalerCSCHal",
        "IOSurfaceScaler", "scaler", NULL
    };
    io_connect_t conn = 0;
    for (int i = 0; names[i] && !conn; i++)
        conn = open_service(names[i], 0);
    if (!conn) { LOG("[scaler] not openable"); return NULL; }

    static int probed = 0;
    if (!probed) { probed = 1; probe_v5(conn, sid); }

    // async fuzz loop (after probe): hammer async methods
    if (!g_wake) {
        mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_wake);
        pthread_t lt;
        pthread_create(&lt, NULL, completion_listener, NULL);
    }
    uint8_t *out = must_map(OUT_SZ);
    static const uint32_t live_sels[] = { 2, 3, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
    for (long round = 0;; round++) {
        craft_request(req, sid);
        uint32_t sel = live_sels[frand() % (sizeof(live_sels)/4)];
        memset(out, 0, OUT_SZ);
        size_t osz = OUT_SZ;
        async_call(conn, sel, req, REQ_SZ, out, &osz, 0, 0);
        sched_yield();
        if ((round & 0x3ff) == 0) usleep(300);
        if ((round & 0x3fff) == 0 && surf) { CFRelease(surf); surf = make_surface(); sid = surf ? IOSurfaceGetID(surf) : 0; }
    }
    return NULL;
}
