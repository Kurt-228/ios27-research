// Target: AppleM2ScalerCSCDriver (rewritten in iOS 27, attached to IOSurfaceRoot)
#include "fuzz.h"
#include <IOSurface/IOSurfaceRef.h>

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

// probe: map selector return codes. For each selector try:
//  (a) struct input 8 bytes, (b) struct input 0x1b0, (c) scalar input only
// Codes meaning (IOKit): 0x0 ok, 0xe00002c2 bad selector/unsupported,
//  0xe00002c7 bad argument count, 0xe00002bc bad argument, 0xe00002f0 not privileged,
//  0xe00002e2 not permitted, 0xe00002c9 exclusive/offline
static void probe(io_connect_t conn, IOSurfaceID sid) {
    uint8_t small[8] = {0};
    uint8_t *req = must_map(REQ_SZ);
    uint64_t scalars[4] = { 1, 2, 3, 4 };
    *(uint64_t *)small = sid;
    LOG("[probe] selector map start");
    for (uint32_t sel = 0; sel < 32; sel++) {
        uint64_t out[16] = {0}; size_t outsz;
        outsz = sizeof(out);
        kern_return_t ka = IOConnectCallMethod(conn, sel, NULL, 0, small, 8, NULL, NULL, out, &outsz);
        size_t osza = outsz;
        craft_request(req, sid);
        outsz = sizeof(out);
        kern_return_t kb = IOConnectCallMethod(conn, sel, NULL, 0, req, REQ_SZ, NULL, NULL, out, &outsz);
        size_t oszb = outsz;
        outsz = sizeof(out);
        kern_return_t kc = IOConnectCallMethod(conn, sel, scalars, 4, NULL, 0, NULL, NULL, out, &outsz);
        LOG("[probe] sel %2u: struct8=0x%08x struct1b0=0x%08x scalar4=0x%08x outsz=%zu/%zu/%zu",
            sel, ka, kb, kc, osza, oszb, outsz);
        usleep(2000);
    }
    LOG("[probe] selector map end");
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
    if (!probed) { probed = 1; probe(conn, sid); }

    for (long round = 0;; round++) {
        craft_request(req, sid);
        uint32_t sel = (uint32_t)frand_range(0, 15);
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
