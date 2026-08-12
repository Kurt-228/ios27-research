// Target: AppleM2ScalerCSCDriver (rewritten in iOS 27, attached to IOSurfaceRoot)
// Sec.32 map: M2ScalerCSCRequest is fed from user struct x21 (~0x1b0 bytes):
//   +0x00: two u32 (pair A)        +0x20: flags bitfield
//   +0x28/0x30/0x38/0x40: fixed-point floats
//   +0x50/0x58: two u64 (surface objs src/dst pair)
//   +0x60/0x68: dims pairs (width,height; zero-checked)
//   +0xa8..0x114: rects/misc, +0x110: 4 records x 0x28
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
    // surface id pair: the driver's surface-map branch (sec.32) needs [req+0xd0]!=0.
    // [req+0xd0]/[req+0xd8/0xe0] sit inside the x21 struct; we don't know their exact
    // offsets yet — spray the real id across the struct at likely spots + random offsets.
    if (sid) {
        *(uint64_t *)(r + 0x50) = sid;
        *(uint64_t *)(r + 0x58) = sid;
        *(uint32_t *)(r + 0xd0) = (uint32_t)sid;          // candidate surface-id field
        if (frand() & 1) {
            size_t off = (frand_range(0, REQ_SZ - 8)) & ~7ULL;
            *(uint64_t *)(r + off) = sid;                 // shotgun: id somewhere else
        }
        if ((frand() & 3) == 0)
            *(uint64_t *)(r + 0xd0) = sid + (uint64_t)frand_range(0, 0x100);
    }
}

static IOSurfaceRef make_surface(void) {
    int w = 64, h = 64, bpe = 4;
    CFNumberRef W = CFNumberCreate(NULL, kCFNumberIntType, &w);
    CFNumberRef H = CFNumberCreate(NULL, kCFNumberIntType, &h);
    CFNumberRef B = CFNumberCreate(NULL, kCFNumberIntType, &bpe);
    int fmt = 0x42475241;
    CFNumberRef F = CFNumberCreate(NULL, kCFNumberIntType, &fmt);
    const void *keys[] = { kIOSurfaceWidth, kIOSurfaceHeight, kIOSurfaceBytesPerElement, kIOSurfacePixelFormat };
    const void *vals[] = { W, H, B, F };
    CFDictionaryRef props = CFDictionaryCreate(NULL, keys, vals, 4,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    IOSurfaceRef s = IOSurfaceCreate(props);
    CFRelease(props); CFRelease(W); CFRelease(H); CFRelease(B); CFRelease(F);
    if (s) LOG("[iosf] IOSurfaceCreate -> id %u", IOSurfaceGetID(s));
    else LOG("[iosf] IOSurfaceCreate failed");
    return s;
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
    if (!conn) {
        LOG("[scaler] not directly openable (unexpected: was openable at first run)");
        return NULL;
    }

    // throttle: stay under symptomsd CPU watchdog (90s/180s -> keep ~45% duty)
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
        // occasionally create/destroy surfaces alongside
        if ((round & 0x3fff) == 0 && surf) { CFRelease(surf); surf = make_surface(); sid = surf ? IOSurfaceGetID(surf) : 0; }
    }
    return NULL;
}
