#ifndef FUZZ_H
#define FUZZ_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOSurface/IOSurfaceTypes.h>
#include <mach/mach.h>
#include <mach/vm_map.h>
// network stack: the last reachable path that hands controlled bytes to the
// kernel (V163). Declared here rather than in the phase because both the
// .m harness and any future C phase need them.
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <errno.h>
// not in iOS SDK headers but present in libsystem
extern kern_return_t mach_vm_region(vm_map_t, mach_vm_address_t *, mach_vm_size_t *,
    vm_region_flavor_t, vm_region_info_t, mach_msg_type_number_t *, mach_port_t *);

// Log line: written to stderr (devicectl console, or Documents/fuzz.log via
// the FUZZ_LOGFILE tee in main.m) AND mirrored to the on-screen live console
// so every phase step is visible on the device itself. Keep the format as a
// C string so callers can use the same macro from .m and C-style code.
void fzlog_emit(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#define LOG(...) fzlog_emit(__VA_ARGS__)

uint64_t frand(void);
uint64_t frand_range(uint64_t lo, uint64_t hi);
void fill_rand(void *buf, size_t n);
void fill_semi_structured(void *buf, size_t n);

io_connect_t open_service(const char *class_name, uint32_t type);
void *must_map(mach_vm_size_t sz);

// targets
void *t_vcpdrm(void *arg);
void *t_iosurface_scaler(void *arg);
void *t_migscan(void *arg);
void *t_xpleak(void *arg);

#endif
