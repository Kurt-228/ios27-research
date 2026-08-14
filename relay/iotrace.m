// V88: in-process IOConnect tracing via __DATA,__interpose (main executable —
// dyld applies it to all later images: Metal.framework's IOKit calls get hooked).
// Originals are resolved via dlsym(RTLD_NEXT) on first call. Recording is gated
// (interpose_set_recording) so app-startup IOKit noise stays out of the ring.
#import <IOKit/IOKitLib.h>
#import <mach/mach.h>
#import <stdio.h>
#import <string.h>
#import <stdarg.h>

// traps are exported by iOS IOKit but marked unavailable in SDK headers —
// reference them through asm aliases (same trick as the fuzzer itself).
extern kern_return_t ref_IOConnectTrap0(io_connect_t, uint32_t) __asm("_IOConnectTrap0");
extern kern_return_t ref_IOConnectTrap1(io_connect_t, uint32_t, uintptr_t) __asm("_IOConnectTrap1");
extern kern_return_t ref_IOConnectTrap2(io_connect_t, uint32_t, uintptr_t, uintptr_t) __asm("_IOConnectTrap2");
extern kern_return_t ref_IOConnectTrap3(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t) __asm("_IOConnectTrap3");
extern kern_return_t ref_IOConnectTrap4(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t) __asm("_IOConnectTrap4");
extern kern_return_t ref_IOConnectTrap5(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t) __asm("_IOConnectTrap5");
extern kern_return_t ref_IOConnectTrap6(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t) __asm("_IOConnectTrap6");

#define RING_N 2048
#define RING_SZ 448
static char g_ring[RING_N][RING_SZ];
static int g_seq;
static int g_rec;

void interpose_set_recording(int on) { g_rec = on; }
void interpose_mark(const char *s) {
    int i = g_seq % RING_N;
    snprintf(g_ring[i], RING_SZ, "#### %s", s);
    g_seq++;
}
void interpose_dump(void (*sink)(const char *)) {
    int n = g_seq < RING_N ? g_seq : RING_N;
    int start = g_seq < RING_N ? 0 : g_seq % RING_N;
    for (int k = 0; k < n; k++) sink(g_ring[(start + k) % RING_N]);
}

static void hexline(char *dst, size_t cap, const uint8_t *p, size_t n) {
    size_t m = 0;
    for (size_t i = 0; i < n && m + 4 < cap; i++)
        m += snprintf(dst + m, cap - m, "%s%02x", i ? " " : "", p[i]);
}

static void rec_method(const char *sym, mach_port_t conn, uint32_t sel,
                       const uint64_t *si, uint32_t sic,
                       const void *stin, size_t stinsz,
                       kern_return_t kr, const uint64_t *so, uint32_t soc,
                       const void *stout, size_t stoutsz) {
    if (!g_rec) return;
    char bin[160] = "", bout[160] = "";
    if (stin && stinsz) hexline(bin, sizeof bin, stin, stinsz > 0x30 ? 0x30 : stinsz);
    if (stout && stoutsz) hexline(bout, sizeof bout, stout, stoutsz > 0x30 ? 0x30 : stoutsz);
    int i = g_seq % RING_N;
    snprintf(g_ring[i], RING_SZ,
             "#%04d %s conn %x sel %u sic %u {%llx %llx %llx %llx} stIn %zx [%s] -> kr %x soc %u {%llx %llx} stOut %zx [%s]",
             g_seq, sym, conn, sel, sic,
             sic > 0 && si ? si[0] : 0, sic > 1 && si ? si[1] : 0,
             sic > 2 && si ? si[2] : 0, sic > 3 && si ? si[3] : 0,
             stinsz, bin, kr, soc,
             soc > 0 && so ? so[0] : 0, soc > 1 && so ? so[1] : 0,
             stoutsz, bout);
    g_seq++;
}

static void rec_simple(const char *fmt, ...) {
    if (!g_rec) return;
    va_list ap; va_start(ap, fmt);
    int i = g_seq % RING_N;
    vsnprintf(g_ring[i], RING_SZ, fmt, ap);
    va_end(ap);
    g_seq++;
}

// NB: dyld never applies an image's interposition to the interposer itself,
// so direct calls below bind to the real IOKit symbols.
typedef kern_return_t (*IOServiceOpen_t)(io_service_t, task_port_t, uint32_t, io_connect_t *);
static kern_return_t my_IOServiceOpen(io_service_t s, task_port_t t, uint32_t type, io_connect_t *c) {
    kern_return_t kr = IOServiceOpen(s, t, type, c);
    io_name_t nm = "?";
    if (s) IOObjectGetClass(s, nm);
    rec_simple("#%04d IOServiceOpen(%s, type 0x%x) -> kr %x conn %x", g_seq, nm, type, kr, c ? *c : 0);
    return kr;
}

typedef kern_return_t (*IOServiceClose_t)(io_connect_t);
static kern_return_t my_IOServiceClose(io_connect_t c) {
    kern_return_t kr = IOServiceClose(c);
    rec_simple("#%04d IOServiceClose(conn %x) -> kr %x", g_seq, c, kr);
    return kr;
}

typedef kern_return_t (*IOConnectCallMethod_t)(mach_port_t, uint32_t, const uint64_t *, uint32_t,
                                               const void *, size_t, uint64_t *, uint32_t *, void *, size_t *);
static kern_return_t my_IOConnectCallMethod(mach_port_t conn, uint32_t sel,
        const uint64_t *si, uint32_t sic, const void *stin, size_t stinsz,
        uint64_t *so, uint32_t *soc, void *stout, size_t *stoutsz) {
    kern_return_t kr = IOConnectCallMethod(conn, sel, si, sic, stin, stinsz, so, soc, stout, stoutsz);
    rec_method("CallMethod", conn, sel, si, sic, stin, stinsz, kr, so, soc ? *soc : 0,
               stout, stoutsz ? *stoutsz : 0);
    return kr;
}

typedef kern_return_t (*IOConnectCallStructMethod_t)(mach_port_t, uint32_t, const void *, size_t, void *, size_t *);
static kern_return_t my_IOConnectCallStructMethod(mach_port_t conn, uint32_t sel,
        const void *stin, size_t stinsz, void *stout, size_t *stoutsz) {
    kern_return_t kr = IOConnectCallStructMethod(conn, sel, stin, stinsz, stout, stoutsz);
    rec_method("CallStruct", conn, sel, NULL, 0, stin, stinsz, kr, NULL, 0,
               stout, stoutsz ? *stoutsz : 0);
    return kr;
}

typedef kern_return_t (*IOConnectCallScalarMethod_t)(mach_port_t, uint32_t, const uint64_t *, uint32_t, uint64_t *, uint32_t *);
static kern_return_t my_IOConnectCallScalarMethod(mach_port_t conn, uint32_t sel,
        const uint64_t *si, uint32_t sic, uint64_t *so, uint32_t *soc) {
    kern_return_t kr = IOConnectCallScalarMethod(conn, sel, si, sic, so, soc);
    rec_method("CallScalar", conn, sel, si, sic, NULL, 0, kr, so, soc ? *soc : 0, NULL, 0);
    return kr;
}

typedef kern_return_t (*IOConnectCallAsyncMethod_t)(mach_port_t, uint32_t, mach_port_t, uint64_t *, uint32_t,
                                                    const uint64_t *, uint32_t, const void *, size_t,
                                                    uint64_t *, uint32_t *, void *, size_t *);
static kern_return_t my_IOConnectCallAsyncMethod(mach_port_t conn, uint32_t sel, mach_port_t wake,
        uint64_t *ref, uint32_t refcnt, const uint64_t *si, uint32_t sic,
        const void *stin, size_t stinsz, uint64_t *so, uint32_t *soc, void *stout, size_t *stoutsz) {
    kern_return_t kr = IOConnectCallAsyncMethod(conn, sel, wake, ref, refcnt, si, sic, stin, stinsz, so, soc, stout, stoutsz);
    rec_method("CallAsync", conn, sel, si, sic, stin, stinsz, kr, so, soc ? *soc : 0,
               stout, stoutsz ? *stoutsz : 0);
    return kr;
}

typedef kern_return_t (*IOConnectSetNotificationPort_t)(io_connect_t, uint32_t, mach_port_t, uintptr_t);
static kern_return_t my_IOConnectSetNotificationPort(io_connect_t c, uint32_t type, mach_port_t p, uintptr_t ref) {
    kern_return_t kr = IOConnectSetNotificationPort(c, type, p, ref);
    rec_simple("#%04d SetNotificationPort(conn %x, type %u, port %x, ref %llx) -> kr %x",
               g_seq, c, type, p, (uint64_t)ref, kr);
    return kr;
}

typedef kern_return_t (*IOConnectMapMemory64_t)(io_connect_t, uint32_t, task_port_t, mach_vm_address_t *, mach_vm_size_t *, IOOptionBits);
static kern_return_t my_IOConnectMapMemory64(io_connect_t c, uint32_t type, task_port_t t,
        mach_vm_address_t *addr, mach_vm_size_t *size, IOOptionBits opts) {
    kern_return_t kr = IOConnectMapMemory64(c, type, t, addr, size, opts);
    rec_simple("#%04d MapMemory64(conn %x, type %u) -> kr %x addr %llx size %llx",
               g_seq, c, type, kr, addr ? *addr : 0, size ? *size : 0);
    return kr;
}

typedef kern_return_t (*IOConnectTrap0_t)(io_connect_t, uint32_t);
static kern_return_t my_IOConnectTrap0(io_connect_t c, uint32_t sel) {
    kern_return_t kr = ref_IOConnectTrap0(c, sel);
    rec_simple("#%04d Trap0(conn %x sel %u) -> kr %x", g_seq, c, sel, kr);
    return kr;
}
typedef kern_return_t (*IOConnectTrap1_t)(io_connect_t, uint32_t, uintptr_t);
static kern_return_t my_IOConnectTrap1(io_connect_t c, uint32_t sel, uintptr_t a1) {
    kern_return_t kr = ref_IOConnectTrap1(c, sel, a1);
    rec_simple("#%04d Trap1(conn %x sel %u {%llx}) -> kr %x", g_seq, c, sel, (uint64_t)a1, kr);
    return kr;
}
typedef kern_return_t (*IOConnectTrap2_t)(io_connect_t, uint32_t, uintptr_t, uintptr_t);
static kern_return_t my_IOConnectTrap2(io_connect_t c, uint32_t sel, uintptr_t a1, uintptr_t a2) {
    kern_return_t kr = ref_IOConnectTrap2(c, sel, a1, a2);
    rec_simple("#%04d Trap2(conn %x sel %u {%llx %llx}) -> kr %x", g_seq, c, sel, (uint64_t)a1, (uint64_t)a2, kr);
    return kr;
}
typedef kern_return_t (*IOConnectTrap3_t)(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t);
static kern_return_t my_IOConnectTrap3(io_connect_t c, uint32_t sel, uintptr_t a1, uintptr_t a2, uintptr_t a3) {
    kern_return_t kr = ref_IOConnectTrap3(c, sel, a1, a2, a3);
    rec_simple("#%04d Trap3(conn %x sel %u {%llx %llx %llx}) -> kr %x", g_seq, c, sel, (uint64_t)a1, (uint64_t)a2, (uint64_t)a3, kr);
    return kr;
}
typedef kern_return_t (*IOConnectTrap4_t)(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
static kern_return_t my_IOConnectTrap4(io_connect_t c, uint32_t sel, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4) {
    kern_return_t kr = ref_IOConnectTrap4(c, sel, a1, a2, a3, a4);
    rec_simple("#%04d Trap4(conn %x sel %u {%llx %llx %llx %llx}) -> kr %x", g_seq, c, sel, (uint64_t)a1, (uint64_t)a2, (uint64_t)a3, (uint64_t)a4, kr);
    return kr;
}
typedef kern_return_t (*IOConnectTrap5_t)(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
static kern_return_t my_IOConnectTrap5(io_connect_t c, uint32_t sel, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5) {
    kern_return_t kr = ref_IOConnectTrap5(c, sel, a1, a2, a3, a4, a5);
    rec_simple("#%04d Trap5(conn %x sel %u {%llx %llx %llx %llx %llx}) -> kr %x", g_seq, c, sel, (uint64_t)a1, (uint64_t)a2, (uint64_t)a3, (uint64_t)a4, (uint64_t)a5, kr);
    return kr;
}
typedef kern_return_t (*IOConnectTrap6_t)(io_connect_t, uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
static kern_return_t my_IOConnectTrap6(io_connect_t c, uint32_t sel, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6) {
    kern_return_t kr = ref_IOConnectTrap6(c, sel, a1, a2, a3, a4, a5, a6);
    rec_simple("#%04d Trap6(conn %x sel %u {%llx %llx %llx %llx %llx %llx}) -> kr %x", g_seq, c, sel, (uint64_t)a1, (uint64_t)a2, (uint64_t)a3, (uint64_t)a4, (uint64_t)a5, (uint64_t)a6, kr);
    return kr;
}

typedef mach_msg_return_t (*mach_msg_t)(mach_msg_header_t *, mach_msg_option_t, mach_msg_size_t,
                                        mach_msg_size_t, mach_port_name_t, mach_msg_timeout_t, mach_port_name_t);
static mach_msg_return_t my_mach_msg(mach_msg_header_t *msg, mach_msg_option_t opt,
        mach_msg_size_t ssz, mach_msg_size_t rsz, mach_port_name_t rname,
        mach_msg_timeout_t to, mach_port_name_t notify) {
    if (g_rec && msg && (opt & MACH_SEND_MSG)) {
        uint32_t id = msg->msgh_id;
        if (id >= 2800 && id < 3200) {   // IOKit MIG subsystem range
            char b[200] = "";
            hexline(b, sizeof b, (const uint8_t *)msg, ssz > 0x60 ? 0x60 : ssz);
            rec_simple("#%04d mach_msg id %u remote %x size %u [%s]", g_seq, id, msg->msgh_remote_port, ssz, b);
        }
    }
    return mach_msg(msg, opt, ssz, rsz, rname, to, notify);
}

typedef struct { const void *repl; const void *orig; } interpose_t;
__attribute__((used)) static const interpose_t interposers[]
    __attribute__((section("__DATA,__interpose"))) = {
    { (const void *)my_mach_msg, (const void *)mach_msg },
    { (const void *)my_IOServiceOpen, (const void *)IOServiceOpen },
    { (const void *)my_IOServiceClose, (const void *)IOServiceClose },
    { (const void *)my_IOConnectCallMethod, (const void *)IOConnectCallMethod },
    { (const void *)my_IOConnectCallStructMethod, (const void *)IOConnectCallStructMethod },
    { (const void *)my_IOConnectCallScalarMethod, (const void *)IOConnectCallScalarMethod },
    { (const void *)my_IOConnectCallAsyncMethod, (const void *)IOConnectCallAsyncMethod },
    { (const void *)my_IOConnectSetNotificationPort, (const void *)IOConnectSetNotificationPort },
    { (const void *)my_IOConnectMapMemory64, (const void *)IOConnectMapMemory64 },
    { (const void *)my_IOConnectTrap0, (const void *)ref_IOConnectTrap0 },
    { (const void *)my_IOConnectTrap1, (const void *)ref_IOConnectTrap1 },
    { (const void *)my_IOConnectTrap2, (const void *)ref_IOConnectTrap2 },
    { (const void *)my_IOConnectTrap3, (const void *)ref_IOConnectTrap3 },
    { (const void *)my_IOConnectTrap4, (const void *)ref_IOConnectTrap4 },
    { (const void *)my_IOConnectTrap5, (const void *)ref_IOConnectTrap5 },
    { (const void *)my_IOConnectTrap6, (const void *)ref_IOConnectTrap6 },
};
