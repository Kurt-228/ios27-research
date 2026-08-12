// Target: AppleM2ScalerCSCDriver — probe v2 (selector arg signatures + sel11 OOB test)
#include "fuzz.h"
#include <IOSurface/IOSurface.h>

#define REQ_SZ 0x1b0

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
        if ((frand() & 3) == 0)
            *(uint64_t *)(r + 0xd0) = sid + (uint64_t)frand_range(0, 0x100);
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

static void probe_v2(io_connect_t conn, IOSurfaceID sid) {
    static const size_t sizes[] = { 0, 4, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 128, 160, 192, 256, 0x1b0, 0x200, 0x400 };
    static const uint32_t sels[] = { 2, 3, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
    uint8_t *req = must_map(0x400);
    craft_request(req, sid);
    LOG("[probe2] size sweep for existing selectors");
    for (unsigned si = 0; si < sizeof(sels)/4; si++) {
        uint32_t sel = sels[si];
        for (unsigned zi = 0; zi < sizeof(sizes)/sizeof(size_t); zi++) {
            size_t z = sizes[zi];
            uint64_t out[16] = {0}; size_t outsz = sizeof(out);
            kern_return_t k = IOConnectCallMethod(conn, sel, NULL, 0, req, z, NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe2] sel %u size %zu -> 0x%08x outsz %zu", sel, z, k, outsz);
            usleep(1500);
        }
    }
    // scalar-count sweep
    uint64_t scalars[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    for (unsigned si = 0; si < sizeof(sels)/4; si++) {
        uint32_t sel = sels[si];
        for (uint32_t n = 0; n <= 8; n++) {
            uint64_t out[16] = {0}; size_t outsz = sizeof(out);
            kern_return_t k = IOConnectCallMethod(conn, sel, scalars, n, NULL, 0, NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe2] sel %u scalarN %u -> 0x%08x outsz %zu", sel, n, k, outsz);
            usleep(1500);
        }
    }
    // sel11 deep OOB test: vary input size, dump output
    LOG("[probe2] sel11 deep test");
    static const size_t s11[] = { 0, 1, 2, 4, 8, 16, 0x1b0, 0x400, 0x1000 };
    for (unsigned zi = 0; zi < sizeof(s11)/sizeof(size_t); zi++) {
        size_t z = s11[zi];
        memset(req, 0, 0x400);
        *(uint32_t *)req = 0x41414141;
        uint64_t out[16] = {0}; size_t outsz = sizeof(out);
        kern_return_t k = IOConnectCallMethod(conn, 11, NULL, 0, req, z, NULL, NULL, out, &outsz);
        LOG("[probe2] sel11 insize %zu -> 0x%08x outsz %zu out=%016llx %016llx %016llx %016llx",
            z, k, outsz, out[0], out[1], out[2], out[3]);
        usleep(2000);
    }
    LOG("[probe2] end");
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
    if (!probed) { probed = 1; probe_v2(conn, sid); }

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
