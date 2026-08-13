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
// not in iOS SDK headers but present in libsystem
extern kern_return_t mach_vm_region(vm_map_t, mach_vm_address_t *, mach_vm_size_t *,
    vm_region_flavor_t, vm_region_info_t, mach_msg_type_number_t *, mach_port_t *);

// devicectl captures stderr from the launched app. Keep the format as a C
// string so callers can use the same macro from .m and C-style code.
#define LOG(...) do { fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)

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

#endif
