// fuzz_metal.m v3 — posix_spawn-per-iteration (Metal не переживает fork(),
// но работает в свежем exec-процессе). Два режима:
//   fmtl3 <seed.metallib> [iters] [seed]   — родитель-цикл
//   fmtl3 --parse <file>                   — одноразовый ребёнок (parse+exit)
// Сборка: cc -arch arm64e -O2 -framework Metal -framework Foundation \
//         fuzz_metal.m -o results/fuzz-logs/fmtl3
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

static uint32_t rs;
static uint32_t xr(void) {
    rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs;
}

static void mutate(uint8_t *buf, size_t n) {
    int rounds = 1 + (xr() % 6);
    for (int r = 0; r < rounds; r++) {
        size_t off = xr() % n;
        switch (xr() % 4) {
        case 0: buf[off] ^= (uint8_t)(1u << (xr() % 8)); break;
        case 1: buf[off] = (uint8_t)(xr() & 0xff); break;
        case 2: buf[off] = 0; break;
        case 3: buf[off] = 0xff; break;
        }
    }
}

static int parse_file(const char *path) {
    @autoreleasepool {
        NSData *d = [NSData dataWithContentsOfFile:[NSString stringWithUTF8String:path]];
        if (!d.length) _exit(4);
        id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
        if (!dev) _exit(3);
        // НАСТОЯЩИЙ dispatch_data (NSData не toll-free с dispatch_data_t!)
        dispatch_data_t dd = dispatch_data_create(d.bytes, d.length, nil, ^{});
        NSError *e = nil;
        id<MTLLibrary> lib = [dev newLibraryWithData:dd error:&e];
        if (lib) {
            for (NSString *fn in [lib functionNames]) {
                id<MTLFunction> f = [lib newFunctionWithName:fn];
                if (f) [f release];
            }
            [lib release];
            _exit(0);
        }
        _exit(1);
    }
}

extern char **environ;

static int spawn_parse(const char *self, const char *file) {
    pid_t pid;
    char *argv[4];
    argv[0] = (char *)self; argv[1] = (char *)"--parse"; argv[2] = (char *)file; argv[3] = NULL;
    if (posix_spawn(&pid, self, NULL, NULL, argv, environ) != 0) return -1;
    int st; waitpid(pid, &st, 0);
    if (WIFEXITED(st)) return WEXITSTATUS(st);   // 0=ok 1=rejected
    if (WIFSIGNALED(st)) return 128 + WTERMSIG(st);  // 128+sig = crash
    return -2;
}

int main(int argc, char **argv) {
    if (argc >= 3 && strcmp(argv[1], "--parse") == 0)
        return parse_file(argv[2]);

    if (argc < 2) { fprintf(stderr, "usage: %s <seed.metallib> [iters] [seed]\n", argv[0]); return 1; }
    unsigned long iters = argc > 2 ? strtoul(argv[2], NULL, 0) : 100000;
    rs = argc > 3 ? (uint32_t)strtoul(argv[3], NULL, 0) : 12345;

    NSData *sd = [NSData dataWithContentsOfFile:[NSString stringWithUTF8String:argv[1]]];
    if (!sd.length) { fprintf(stderr, "seed не читается\n"); return 1; }
    size_t n = sd.length;
    uint8_t *seed = malloc(n); memcpy(seed, sd.bytes, n);
    uint8_t *work = malloc(n);
    const char *tmpf = "/tmp/fmtl3-input.bin";
    char selfpath[4096];
    uint32_t sz = sizeof(selfpath) - 1;
    _NSGetExecutablePath(selfpath, &sz); selfpath[sz] = 0;

    fprintf(stderr, "seed: %zu байт, iters=%lu, helper=%s\n", n, iters, selfpath);

    // контроль §144: немут. сид
    FILE *f = fopen(tmpf, "wb"); fwrite(seed, 1, n, f); fclose(f);
    int rc0 = spawn_parse(selfpath, tmpf);
    fprintf(stderr, "контроль (немут. сид): rc=%d (%s)\n", rc0,
            rc0 == 0 ? "OK — парсер принял" : rc0 <= 100 ? "rejected" : "CRASH — сид крашит!");
    if (rc0 > 100) return 3;  // сид сам крашит — не фаззим

    unsigned long total = 0, ok = 0, err = 0, crashes = 0;
    for (unsigned long it = 0; it < iters; it++) {
        total++;
        memcpy(work, seed, n);
        mutate(work, n);
        f = fopen(tmpf, "wb"); fwrite(work, 1, n, f); fclose(f);
        int rc = spawn_parse(selfpath, tmpf);
        if (rc == 0) ok++;
        else if (rc >= 1 && rc <= 100) err++;
        else {
            crashes++;
            char name[128];
            snprintf(name, sizeof(name), "/tmp/fmtl3-crash-%lu.bin", total);
            FILE *cf = fopen(name, "wb"); fwrite(work, 1, n, cf); fclose(cf);
            fprintf(stderr, "CRASH iter=%lu rc=%d saved=%s\n", total, rc, name);
            if (crashes >= 50) { fprintf(stderr, "много крашей — стоп\n"); break; }
        }
        if ((total % 1000) == 0)
            fprintf(stderr, "[%lu] ok=%lu err=%lu crash=%lu\n", total, ok, err, crashes);
    }
    printf("done: %lu iters, ok=%lu err=%lu crash=%lu\n", total, ok, err, crashes);
    return crashes ? 2 : 0;
}
