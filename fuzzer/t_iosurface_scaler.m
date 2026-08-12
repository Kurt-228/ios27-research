// Target: AppleM2ScalerCSCDriver — probe v7 (exact-size methods + notification port)
// Static analysis of com.apple.driver.AppleM2ScalerCSCDriver (27.0b4) found the
// real method table in __DATA_CONST @0xfffffff007f75848: 11 methods with exact
// input sizes: {-1, 0x1b0, 0, 0, 0x20, ~0, 0xfa8, 8, 8, 0x10, 0x18}.
// Methods are async-registered: return kIOReturnNoCompletion and complete later
// via sendAsyncResult64 to the connection's notification port.
// v7: IOConnectSetNotificationPort + exact-size sync/async calls + full kr log.
#include "fuzz.h"
#include <IOSurface/IOSurfaceRef.h>
#include <stdarg.h>

#define BIG_SZ 0x2000

static mach_port_t g_wake;      // per-call async port
static mach_port_t g_notify;    // connection notification port

static void *msg_listener(void *arg) {
    mach_port_t port = (mach_port_t)(uintptr_t)arg;
    struct { mach_msg_header_t h; uint8_t data[0x400]; } msg;
    long timeouts = 0;
    for (;;) {
        kern_return_t kr = mach_msg(&msg.h, MACH_RCV_MSG | MACH_RCV_TIMEOUT, 0,
                                    sizeof(msg), port, 2000, MACH_PORT_NULL);
        if (kr == MACH_RCV_TIMED_OUT) {
            if (++timeouts % 30 == 0) LOG("[rx 0x%x] alive", port);
            continue;
        }
        if (kr) { LOG("[rx 0x%x] err 0x%x", port, kr); continue; }
        uint32_t *d = (uint32_t *)&msg;
        LOG("[rx 0x%x] MSG id 0x%x size %u bits 0x%x: %08x %08x %08x %08x %08x %08x %08x %08x",
            port, msg.h.msgh_id, msg.h.msgh_size, msg.h.msgh_bits,
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

// methods discovered statically (order in method table):
//  m0 size any (stub)   m1 0x1b0 submit-desc   m2 0 (stub)   m3 0 (stub)
//  m4 0x20              m5 getter (no input)  m6 0xfa8 batch (u32 count <= 0x3e8)
//  m7 8                 m8 8                  m9 0x10        m10 0x18 (u32 < 4)
// selector base unknown (probably 0-3 offset from inherited IOUserClient2022 methods)
static const uint32_t meth_sizes[] = { 0, 0x1b0, 0, 0, 0x20, 0, 0xfa8, 8, 8, 0x10, 0x18 };
#define NMETH 11

static void craft_sized(uint8_t *r, size_t sz, IOSurfaceID sid, uint32_t sel_hint) {
    fill_semi_structured(r, sz);
    if (sz >= 8 && (frand() & 1)) *(uint64_t *)(r + 8) = 0;      // m1 path B trigger
    if (sz == 0xfa8) *(uint32_t *)r = 1 + (frand() % 3);        // m6 count
    if (sz == 0x18) *(uint32_t *)r = frand() % 4;               // m10 enum
    if (sz == 8 && sid) *(uint64_t *)r = sid;                   // m7/m8 surface id
    if (sz == 0x1b0 && sid) {
        *(uint64_t *)(r + 0x50) = sid;
        *(uint64_t *)(r + 0x58) = sid;
    }
}

static void probe_v7(io_connect_t conn, IOSurfaceID sid) {
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_wake);
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_notify);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, msg_listener, (void *)(uintptr_t)g_wake);
    pthread_create(&t2, NULL, msg_listener, (void *)(uintptr_t)g_notify);

    kern_return_t kr = IOConnectSetNotificationPort(conn, 0, g_notify, 0);
    LOG("[probe7] IOConnectSetNotificationPort -> 0x%08x", kr);

    uint8_t *req = must_map(BIG_SZ);
    uint8_t *out = must_map(BIG_SZ);

    // selector base discovery: call sel 0..15 with each plausible exact size
    static const uint32_t try_sizes[] = { 0, 8, 0x10, 0x18, 0x20, 0x1b0, 0xfa8 };
    for (uint32_t sel = 0; sel <= 15; sel++) {
        for (unsigned zi = 0; zi < sizeof(try_sizes)/4; zi++) {
            size_t sz = try_sizes[zi];
            memset(req, 0, BIG_SZ);
            if (sz) craft_sized(req, sz, sid, sel);
            memset(out, 0xAA, BIG_SZ);
            size_t osz = BIG_SZ;
            uint64_t osc[2] = {0,0};
            uint32_t nosc = 0;
            kr = IOConnectCallMethod(conn, sel, NULL, 0, sz ? req : NULL, sz,
                                     osc, &nosc, out, &osz);
            if (kr != 0xe00002c2 && kr != 0xe00002c7 && kr != 0xe00002bc)
                LOG("[probe7] sync sel %2u sz 0x%-4zx -> kr 0x%08x osz %zu out0-3 %02x %02x %02x %02x",
                    sel, sz, kr, osz, out[0], out[1], out[2], out[3]);
        }
    }
    LOG("[probe7] sync sweep done; waiting 5s on wake/notify ports");
    usleep(5000000);

    // async with exact sizes across sel 0..13
    for (uint32_t sel = 0; sel <= 13; sel++) {
        for (unsigned mi = 0; mi < NMETH; mi++) {
            size_t sz = meth_sizes[mi];
            memset(req, 0, BIG_SZ);
            if (sz) craft_sized(req, sz, sid, sel);
            memset(out, 0xAA, BIG_SZ);
            size_t osz = BIG_SZ;
            uint64_t refs[1] = { 0x41414141 };
            uint64_t osc[2] = {0,0};
            uint32_t nosc = 0;
            kr = IOConnectCallAsyncMethod(conn, sel, g_wake, refs, 1,
                                          NULL, 0, sz ? req : NULL, sz,
                                          osc, &nosc, out, &osz);
            LOG("[probe7] async sel %2u sz 0x%-4zx -> kr 0x%08x osz %zu", sel, sz, kr, osz);
            usleep(2000);
        }
    }
    LOG("[probe7] async sweep done; waiting 8s for completions");
    usleep(8000000);
    LOG("[probe7] end");
}

void *t_iosurface_scaler(void *arg) {
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
    LOG("[scaler] conn 0x%x, sid %u", conn, sid);

    static int probed = 0;
    if (!probed) { probed = 1; probe_v7(conn, sid); }

    // throttled semantic fuzz on exact-size methods
    if (!g_wake) {
        mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &g_wake);
        pthread_t t; pthread_create(&t, NULL, msg_listener, (void *)(uintptr_t)g_wake);
    }
    uint8_t *req = must_map(BIG_SZ);
    uint8_t *out = must_map(BIG_SZ);
    for (long round = 0;; round++) {
        uint32_t sel = frand() % 14;
        uint32_t mi = frand() % NMETH;
        size_t sz = meth_sizes[mi];
        memset(req, 0, BIG_SZ);
        if (sz) craft_sized(req, sz, sid, sel);
        memset(out, 0xAA, BIG_SZ);
        size_t osz = BIG_SZ;
        uint64_t osc[2] = {0,0}; uint32_t nosc = 0;
        kern_return_t kr;
        if (frand() & 1) {
            uint64_t refs[1] = { frand() };
            kr = IOConnectCallAsyncMethod(conn, sel, g_wake, refs, 1,
                                          NULL, 0, sz ? req : NULL, sz,
                                          osc, &nosc, out, &osz);
        } else {
            kr = IOConnectCallMethod(conn, sel, NULL, 0, sz ? req : NULL, sz,
                                     osc, &nosc, out, &osz);
        }
        if (kr == 0 || (kr != 0xe00002c2 && kr != 0xe00002c7 && kr != 0xe00002bc && kr != 0xe00002bf && (round & 0x3f) == 0))
            LOG("[fuzz] r%ld sel %u sz 0x%zx kr 0x%08x osz %zu", round, sel, sz, kr, osz);
        usleep(150 + (frand() & 0x7f));
        if ((round & 0x3fff) == 0 && surf) { CFRelease(surf); surf = make_surface(); sid = surf ? IOSurfaceGetID(surf) : 0; }
    }
    return NULL;
}
