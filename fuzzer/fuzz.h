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

#define LOG(...) do { NSLog(__VA_ARGS__); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)

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
