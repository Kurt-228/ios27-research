// Target: AppleM2ScalerCSCDriver — probe v4 (structureOutputSize hypothesis)
// Model: external methods may require an OOL structureOutputDescriptor of exact
// size (x3 in target fn has +0x15c count field -> likely the output struct).
// Our earlier sweeps never went above 256 bytes of structureOutput.
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

static void probe_v4(io_connect_t conn, IOSurfaceID sid) {
    static const uint32_t sels[] = { 2, 3, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
    static const size_t outsizes[] = { 0x28, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x2380, 0x4000, 0x8000 };
    uint8_t *req = must_map(REQ_SZ);
    uint8_t *out = must_map(0x8000);
    uint64_t scalars[4] = { 1, 2, 3, 4 };
    LOG("[probe4] structureOutputSize sweep");
    for (unsigned si = 0; si < sizeof(sels)/4; si++) {
        uint32_t sel = sels[si];
        for (unsigned oi = 0; oi < sizeof(outsizes)/sizeof(size_t); oi++) {
            craft_request(req, sid);
            memset(out, 0, 0x8000);
            size_t outsz = outsizes[oi];
            kern_return_t k = IOConnectCallMethod(conn, sel, NULL, 0, req, REQ_SZ,
                                                  NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe4] sel %u outsweep 0x%zx -> 0x%08x got 0x%zx first16=%016llx %016llx",
                    sel, outsizes[oi], k, outsz, *(uint64_t *)out, *(uint64_t *)(out + 8));
            usleep(1500);
        }
        // scalarN with big output
        for (uint32_t n = 0; n <= 4; n++) {
            craft_request(req, sid);
            memset(out, 0, 0x8000);
            size_t outsz = 0x1000;
            kern_return_t k = IOConnectCallMethod(conn, sel, scalars, n, req, REQ_SZ,
                                                  NULL, NULL, out, &outsz);
            if (k != 0xe00002c7 && k != 0xe00002c2)
                LOG("[probe4] sel %u scalarN %u -> 0x%08x got 0x%zx", sel, n, k, outsz);
            usleep(1500);
        }
        usleep(3000);
    }
    LOG("[probe4] end");
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
    if (!probed) { probed = 1; probe_v4(conn, sid); }

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
