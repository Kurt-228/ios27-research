#include "fuzz.h"

static uint64_t s;
uint64_t frand(void) {
    if (!s) s = 0x243F6A8885A308D3ULL ^ (uint64_t)pthread_self();
    s ^= s << 13; s ^= s >> 7; s ^= s << 17;
    return s;
}
uint64_t frand_range(uint64_t lo, uint64_t hi) {
    if (hi <= lo) return lo;
    return lo + frand() % (hi - lo + 1);
}
void fill_rand(void *buf, size_t n) {
    uint8_t *p = buf;
    for (size_t i = 0; i < n; i += 8) {
        uint64_t v = frand();
        size_t c = (n - i) < 8 ? (n - i) : 8;
        memcpy(p + i, &v, c);
    }
}
// structured-ish: mix of small ints, ascii, 0x4141.., real-lookng floats/dims
void fill_semi_structured(void *buf, size_t n) {
    uint8_t *p = buf;
    static const uint64_t patterns[] = {
        0, 1, 2, 3, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x400, 0x1000, 0x4000,
        0x10000, 0x100000, 0x780, 0x438, 0x1920, 0x1080, 0x4141414141414141,
        0x7f7f7f7f7f7f7f7f, 0xffffffff, 0xffffffffffffffff, 0x80000000,
        0x3ff0000000000000, 0x4000000000000000, 0x4059000000000000,
        0x7fefffffffffffff, 0xfff0000000000000,
    };
    for (size_t i = 0; i < n; i += 8) {
        uint64_t v;
        switch (frand() % 4) {
            case 0: v = patterns[frand() % (sizeof(patterns)/8)]; break;
            case 1: v = frand_range(0, 0x2000); break;
            case 2: v = frand(); break;
            default: v = patterns[frand() % (sizeof(patterns)/8)] ^ frand(); break;
        }
        memcpy(p + i, &v, (n - i) < 8 ? (n - i) : 8);
    }
}
io_connect_t open_service(const char *class_name, uint32_t type) {
    CFMutableDictionaryRef m = IOServiceMatching(class_name);
    if (!m) { LOG("[open] no matching dict for %s", class_name); return 0; }
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, m);
    if (!svc) { LOG("[open] service %s not found", class_name); return 0; }
    io_connect_t conn = 0;
    kern_return_t kr = IOServiceOpen(svc, mach_task_self(), type, &conn);
    IOObjectRelease(svc);
    if (kr || !conn) { LOG("[open] IOServiceOpen %s failed: 0x%x", class_name, kr); return 0; }
    LOG("[open] %s -> conn 0x%x", class_name, conn);
    return conn;
}
void *must_map(mach_vm_size_t sz) {
    vm_address_t a = 0;
    vm_size_t size = (vm_size_t)sz;
    if (vm_allocate(mach_task_self(), &a, size, VM_FLAGS_ANYWHERE)) {
        LOG("[map] alloc failed"); exit(1);
    }
    return (void *)a;
}
