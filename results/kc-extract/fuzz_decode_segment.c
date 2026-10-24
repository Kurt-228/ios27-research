// fuzz_decode_segment — Mac-harness: вызываем CA::OGL::MetalContext::decode_segment
// напрямую из извлечённого QuartzCore с мутациями блоба functions.data.
// Цель: найти memory corruption в том же коде, что читает кэш в backboardd (iOS).
//
// Механика:
//  - dlopen qc_mac.dylib (извлечён из macOS dsc официально dsc_extractor);
//  - dlsym __ZN2CA3OGL12MetalContext14decode_segmentEPPKcPvPKNS1_11NameDecoderEi;
//  - NameDecoder-таблица FragmentShader::Spec: VM 0x1e886ec60 (из disasm) →
//    runtime = image_base + (0x1e886ec60 - min_vmaddr) — считаем через dladdr;
//  - dest = mmap-буфер 64KB + GUARD-PAGE (PROT_NONE) сразу за ним:
//    линейный overflow за пределы dest = мгновенный SIGSEGV;
//  - вход: реальный блоб из functions.data (вырезан с устройства, §264) +
//    побайтовые/bit-flip мутации; cursor = const char* внутрь буфера.
//
// ВАЖНО: decode_segment(cursor_ptr, dest, table, nfields=20) — сигнатура из
// disasm: (const char **cursor, void *dest, const NameDecoder *tbl, int n).
// Сборка: cc -arch arm64e fuzz_decode_segment.c -o /tmp/fds -framework Foundation
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdint.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>

typedef void (*decode_segment_t)(const char **cursor, void *dest,
                                  const void *tbl, int n);

static void *g_dest;          // 64KB + guard
static decode_segment_t g_fn;
static const void *g_tbl;
static sigjmp_buf g_jmp;
static volatile sig_atomic_t g_hit;

static void segv_handler(int sig) {
    g_hit = 1;
    siglongjmp(g_jmp, 1);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "usage: %s <qc_mac.dylib> <blob.bin> <table_vmaddr_hex>\n", argv[0]);
        return 1;
    }
    void *dl = dlopen(argv[1], RTLD_NOW);
    if (!dl) { fprintf(stderr, "dlopen: %s\n", dlerror()); return 1; }
    g_fn = (decode_segment_t)dlsym(dl,
        "__ZN2CA3OGL12MetalContext14decode_segmentEPPKcPvPKNS1_11NameDecoderEi");
    if (!g_fn) { fprintf(stderr, "dlsym decode_segment: %s\n", dlerror()); return 1; }
    // база образа: dladdr любого символа
    Dl_info info = {0};
    if (!dladdr(g_fn, &info) || !info.dli_fbase) { fprintf(stderr, "dladdr fail\n"); return 1; }
    // min_vmaddr __TEXT = 0x18af36000 (из disasm qc_mac.dylib)
    uint64_t text_vm = 0x18af36000ULL;
    uint64_t want_tbl = strtoull(argv[3], NULL, 16);
    g_tbl = (const void *)((uint64_t)info.dli_fbase + (want_tbl - text_vm));

    // blob
    FILE *f = fopen(argv[2], "rb");
    if (!f) { perror("blob"); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0 || sz > 1 << 20) { fprintf(stderr, "blob size %ld\n", sz); return 1; }
    uint8_t *blob = malloc((size_t)sz + 4096);
    if (fread(blob, 1, (size_t)sz, f) != (size_t)sz) { perror("read"); return 1; }
    fclose(f);

    // dest: 64KB полезных + guard
    g_dest = mmap(NULL, 64 * 1024 + 4096, PROT_READ | PROT_WRITE,
                  MAP_ANON | MAP_PRIVATE, -1, 0);
    mprotect((char *)g_dest + 64 * 1024, 4096, PROT_NONE);

    signal(SIGSEGV, segv_handler);
    signal(SIGBUS, segv_handler);

    uint64_t iters = argc > 4 ? strtoull(argv[4], NULL, 0) : 1000000;
    uint32_t seed = (uint32_t)getpid() * 1103515245u + 12345u;
    long crashes = 0;
    for (uint64_t it = 0; it < iters; it++) {
        // мутация: 1-4 случайных байтовых изменений в первых 512 байтах блоба
        uint8_t *work = blob + (rand_r(&seed) % (sz > 512 ? sz - 512 : 1));
        memcpy(g_dest, (char *)g_dest /*noise unimportant*/, 1); // keep warm
        memset(g_dest, 0, 64 * 1024);
        int nmut = 1 + (rand_r(&seed) % 4);
        for (int m = 0; m < nmut; m++) {
            int off = rand_r(&seed) % (sz > 600 ? 600 : sz);
            work[off] ^= (uint8_t)(1u << (rand_r(&seed) % 8));
        }
        const char *cursor = (const char *)blob;
        if (sigsetjmp(g_jmp, 1) == 0) {
            g_fn(&cursor, g_dest, g_tbl, 20);
        } else {
            crashes++;
            fprintf(stderr, "CRASH @iter %llu (dest-guard hit)\n",
                    (unsigned long long)it);
            // дамп блоба
            char name[64];
            snprintf(name, sizeof(name), "/tmp/fds-crash-%llu.bin",
                     (unsigned long long)it);
            FILE *o = fopen(name, "wb");
            if (o) { fwrite(blob, 1, (size_t)sz, o); fclose(o); }
            if (crashes > 20) break;
        }
    }
    printf("done: %llu итераций, %ld сигналов\n",
           (unsigned long long)iters, crashes);
    return crashes ? 2 : 0;
}
