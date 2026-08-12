// Target: AppleM2ScalerCSCDriver — probe v3 (exhaustive arg mapping + sel11 state diff)
#include "fuzz.h"
#include <IOSurface/IOSurface.h>

#define REQ_SZ 0x1b0

static void craft_request(uint8_t *r, IOSurfaceID sid) {
    fill_semi_structured(r, REQ_SZ);
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

static void dump11(io_connect_t conn, const char *tag) {
    uint8_t in[8] = {0};
    uint64_t out[16] = {0}; size_t outsz = sizeof(out);
    kern_return_t k = IOConnectCallMethod(conn, 11, NULL, 0, in, 0, NULL, NULL, out, &outsz);
    LOG("[dump11 %s] k=0x%08x out=%016llx %016llx %016llx %016llx %016llx %016llx",
        tag, k, out[0], out[1], out[2], out[3], out[4], out[5]);
}

static void probe_v3(io_connect_t conn, IOSurfaceID sid) {
    static const uint32_t sels[] = { 2, 3, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
    uint8_t *req = must_map(0x400);
    uint64_t scalars[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    LOG("[probe3] exhaustive size sweep 4..0x400 step 4");
    for (unsigned si = 0; si < sizeof(sels)/4; si++) {
        uint32_t sel = sels[si];
        for (size_t z = 4; z <= 0x400; z += 4) {
            craft_request(req, sid);
            uint64_t out[16] = {0}; size_t outsz = sizeof(out);
            kern_return_t k = IOConnectCallMethod(conn, sel, NULL, 0, req, z, NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe3] sel %u struct %zu -> 0x%08x outsz %zu", sel, z, k, outsz);
        }
        // combined scalar+struct (2 scalars + struct)
        for (size_t z = 8; z <= 0x200; z += 8) {
            craft_request(req, sid);
            uint64_t out[16] = {0}; size_t outsz = sizeof(out);
            kern_return_t k = IOConnectCallMethod(conn, sel, scalars, 2, req, z, NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe3] sel %u scalar2+struct %zu -> 0x%08x outsz %zu", sel, z, k, outsz);
        }
        // structureOutputSize sweep with fixed struct 0x1b0
        static const size_t oszs[] = { 0, 4, 8, 16, 24, 32, 40, 48, 64, 96, 128, 256 };
        for (unsigned oi = 0; oi < sizeof(oszs)/sizeof(size_t); oi++) {
            craft_request(req, sid);
            uint64_t out[32] = {0}; size_t outsz = oszs[oi] > sizeof(out) ? sizeof(out) : oszs[oi];
            kern_return_t k = IOConnectCallMethod(conn, sel, NULL, 0, req, REQ_SZ, NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe3] sel %u outsweep %zu -> 0x%08x got %zu", sel, oszs[oi], k, outsz);
        }
        usleep(3000);
    }
    LOG("[probe3] sweep end");

    // sel11 state-diff: dump around attempts on sel 2 and 3 with legal-looking id
    dump11(conn, "before");
    for (uint32_t sel = 2; sel <= 3; sel++) {
        for (uint32_t id = 1; id <= 0x20; id++) {
            uint64_t out[16] = {0}; size_t outsz = sizeof(out);
            uint64_t v = id;
            kern_return_t k = IOConnectCallMethod(conn, sel, NULL, 0, &v, 8, NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe3] sel%u id%u -> 0x%08x", sel, id, k);
        }
    }
    dump11(conn, "after");
    LOG("[probe3] end");
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
    if (!probed) { probed = 1; probe_v3(conn, sid); }

    for (long round = 0;; round++) {
        craft_request(req, sid);
        uint32_t sel = (uint32_t)frand_range(2, 31);
        uint64_t out[16] = {0}; size_t outsz = sizeof(out);
        kern_return_t kr = IOConnectCallMethod(conn, sel, NULL, 0,
            req, REQ_SZ, NULL, NULL, out, &outsz);
        if (kr && kr != 0xe00002c2 && kr != 0xe00002c7 && kr != 0xe00002c9 &&
            kr != 0xe00002bc && kr != 0xe00002f0 && kr != 0xe00002e2)
            LOG("[scaler] sel %u -> 0x%x", sel, kr);
        sched_yield();
        if ((round & 0x3ff) == 0) usleep(300);
        if ((round & 0x3fff) == 0 && surf) { CFRelease(surf); surf = make_surface(); sid = surf ? IOSurfaceGetID(surf) : 0; }
    }
    return NULL;
}
