// Target: AppleM2ScalerCSCDriver (rewritten in iOS 27, attached to IOSurfaceRoot)
// Sec.32 map: M2ScalerCSCRequest is fed from user struct x21 (~0x1b0 bytes):
//   +0x00: two u32 (pair A)        +0x20: flags bitfield
//   +0x28/0x30/0x38/0x40: fixed-point floats (w1=0x2f/0 converters)
//   +0x50/0x58: two u64 (surface objs src/dst pair)
//   +0x60/0x68: dims pairs (width,height; zero-checked)
//   +0xa8..0x114: rects/misc, +0x110: 4 records x 0x28
// We fuzz that struct through every external method index we can open.
#include "fuzz.h"

#define REQ_SZ 0x1b0

static void craft_request(uint8_t *r, IOSurfaceID sid) {
    fill_semi_structured(r, REQ_SZ);
    // sane dimensions in half the cases (reach deeper code)
    if (frand() & 1) {
        *(uint32_t *)(r + 0x68) = (uint32_t)frand_range(1, 4096);
        *(uint32_t *)(r + 0x6c) = (uint32_t)frand_range(1, 4096);
        *(uint32_t *)(r + 0x60) = 0x3f800000; // 1.0f
        *(uint32_t *)(r + 0x64) = 0x3f800000;
    }
    // surface id pair: real IOSurface id in some cases (valid or +noise)
    if (sid && (frand() % 3)) {
        *(uint64_t *)(r + 0x50) = sid;
        *(uint64_t *)(r + 0x58) = sid;
        if ((frand() & 3) == 0) *(uint64_t *)(r + 0x50) = sid + (uint64_t)frand_range(0, 0x100);
    }
}

static IOSurfaceID make_surface(void) {
    // IOSurfaceRootUserClient sel 0 = create (legacy path still present)
    io_connect_t root = open_service("IOSurfaceRootUserClient", 0);
    if (!root) return 0;
    uint32_t dict_ver = 0;
    CFMutableDictionaryRef props = CFDictionaryCreateMutable(NULL, 0,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    int w = 64, h = 64, bpe = 4;
    int32_t fmt = 0x42475241; // 'BGRA'
    CFDictionarySetValue(props, CFSTR("IOSurfaceWidth"),
        CFNumberCreate(NULL, kCFNumberIntType, &w));
    CFDictionarySetValue(props, CFSTR("IOSurfaceHeight"),
        CFNumberCreate(NULL, kCFNumberIntType, &h));
    CFDictionarySetValue(props, CFSTR("IOSurfaceBytesPerElement"),
        CFNumberCreate(NULL, kCFNumberIntType, &bpe));
    CFDictionarySetValue(props, CFSTR("IOSurfacePixelFormat"),
        CFNumberCreate(NULL, kCFNumberIntType, &fmt));
    uint64_t id = 0; size_t idsz = 8;
    kern_return_t kr = IOConnectCallMethod(root, 0, NULL, 0,
        props, CFPropertyListCreateData(NULL, props, kCFPropertyListBinaryFormat_v1_0, 0, NULL).length,
        NULL, NULL, &id, &idsz);
    // fallback: binary struct path
    if (kr) {
        struct { uint32_t ver; uint32_t w; uint32_t h; uint32_t bpe; uint32_t fmt; } in =
            { 0, 64, 64, 4, 0x42475241 };
        idsz = sizeof(id);
        kr = IOConnectCallMethod(root, 0, NULL, 0, &in, sizeof(in), NULL, NULL, &id, &idsz);
    }
    IOObjectRelease(root);
    if (kr || !id) { LOG("[iosf] surface create failed 0x%x", kr); return 0; }
    LOG("[iosf] surface id 0x%llx", id);
    return (IOSurfaceID)id;
}

void *t_iosurface_scaler(void *arg) {
    uint8_t *req = must_map(REQ_SZ);
    IOSurfaceID sid = make_surface();

    // try every plausible service name for the scaler
    const char *names[] = {
        "AppleM2ScalerCSCDriver", "AppleM2Scaler", "AppleM2ScalerCSCHal",
        "IOSurfaceScaler", "scaler", NULL
    };
    io_connect_t conn = 0;
    for (int i = 0; names[i] && !conn; i++)
        conn = open_service(names[i], 0);
    if (!conn) {
        LOG("[scaler] not directly openable; enumerating IOSurfaceRoot children for manual map");
        io_iterator_t it = 0;
        IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceMatching("IOService"), &it);
        io_service_t s;
        int n = 0;
        while ((s = IOIteratorNext(it)) && n < 400) {
            io_name_t nm;
            IORegistryEntryGetName(s, nm);
            if (strstr(nm, "caler") || strstr(nm, "MSR") || strstr(nm, "M2"))
                LOG("[enum] candidate service: %s", nm);
            IOObjectRelease(s); n++;
        }
        IOObjectRelease(it);
        return NULL;
    }

    for (long round = 0;; round++) {
        craft_request(req, sid);
        uint32_t sel = (uint32_t)frand_range(0, 15);
        uint64_t out[16] = {0}; size_t outsz = sizeof(out);
        kern_return_t kr = IOConnectCallMethod(conn, sel, NULL, 0,
            req, REQ_SZ, NULL, NULL, out, &outsz);
        if (kr && kr != 0xe00002c2 && kr != 0xe00002c9 && kr != 0xe00002bc && kr != 0xe00002f0)
            LOG("[scaler] sel %u -> 0x%x", sel, kr);
        if ((round & 0x7ff) == 0) usleep(500);
    }
    return NULL;
}
