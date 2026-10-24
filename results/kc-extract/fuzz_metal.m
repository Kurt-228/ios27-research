// fuzz_metal.m v4 — batch + blacklist:
//   ребёнок (--batch <dir>): парсит variantNNN.bin по порядку, перед каждым
//   пишет текущий индекс в <dir>/cursor; краш = индекс известен.
//   родитель: генерирует пачки по 20, спавнит ребёнка, читает cursor.
//   Blacklist: оффсеты, чьи мутации дают assert-краш, исключаются дальше
//   (в памяти родителя) → мутации уходят в новые поля.
// Сборка: cc -arch arm64e -O2 -framework Metal -framework Foundation \
//         fuzz_metal.m -o results/fuzz-logs/fmtl4
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

static uint32_t rs;
static uint32_t xr(void) {
    rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs;
}

static void mutate(uint8_t *buf, size_t n, unsigned char *black) {
    int rounds = 1 + (xr() % 6);
    for (int r = 0; r < rounds; r++) {
        size_t off = xr() % n;
        if (black[off]) { r--; continue; }   // skip blacklisted
        switch (xr() % 4) {
        case 0: buf[off] ^= (uint8_t)(1u << (xr() % 8)); break;
        case 1: buf[off] = (uint8_t)(xr() & 0xff); break;
        case 2: buf[off] = 0; break;
        case 3: buf[off] = 0xff; break;
        }
    }
}

static int parse_one(const char *path) {
    @autoreleasepool {
        NSData *d = [NSData dataWithContentsOfFile:[NSString stringWithUTF8String:path]];
        if (!d.length) return 4;
        id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
        if (!dev) return 3;
        dispatch_data_t dd = dispatch_data_create(d.bytes, d.length, nil, ^{});
        NSError *e = nil;
        id<MTLLibrary> lib = [dev newLibraryWithData:dd error:&e];
        if (lib) {
            for (NSString *fn in [lib functionNames]) {
                id<MTLFunction> f = [lib newFunctionWithName:fn];
                if (f) [f release];
            }
            [lib release];
            return 0;
        }
        return 1;
    }
}

extern char **environ;

static int run_batch(const char *dir, int count) {
    // ребёнок: последовательный парсинг, курсор в файл
    char cur[600]; snprintf(cur, sizeof(cur), "%s/cursor", dir);
    for (int i = 0; i < count; i++) {
        FILE *cf = fopen(cur, "w");
        if (cf) { fprintf(cf, "%d", i); fclose(cf); }
        char vp[600]; snprintf(vp, sizeof(vp), "%s/variant%03d.bin", dir, i);
        int rc = parse_one(vp);
        if (rc != 0 && rc != 1) _exit(rc);   // 4/3 = фатальные
    }
    FILE *cf = fopen(cur, "w"); if (cf) { fprintf(cf, "%d", count); fclose(cf); }
    _exit(0);
}

int main(int argc, char **argv) {
    if (argc >= 3 && strcmp(argv[1], "--batch") == 0)
        return run_batch(argv[2], atoi(argv[3]));

    if (argc < 2) { fprintf(stderr, "usage: %s <seed.metallib> [iters] [seed]\n", argv[0]); return 1; }
    unsigned long iters = argc > 2 ? strtoul(argv[2], NULL, 0) : 1000000;
    rs = argc > 3 ? (uint32_t)strtoul(argv[3], NULL, 0) : 12345;

    NSData *sd = [NSData dataWithContentsOfFile:[NSString stringWithUTF8String:argv[1]]];
    if (!sd.length) { fprintf(stderr, "seed не читается\n"); return 1; }
    size_t n = sd.length;
    uint8_t *seed = malloc(n); memcpy(seed, sd.bytes, n);
    unsigned char *black = calloc(1, n);

    char selfpath[4096]; uint32_t sz = sizeof(selfpath)-1;
    _NSGetExecutablePath(selfpath, &sz); selfpath[sz] = 0;
    const char *dir = "/tmp/fmtl4-batch";
    mkdir(dir, 0777);

    fprintf(stderr, "seed: %zu байт, iters=%lu, BATCH=20\n", n, iters);
    // контроль
    {
        char vp[600]; snprintf(vp, sizeof(vp), "%s/variant000.bin", dir);
        FILE *f = fopen(vp, "wb"); fwrite(seed, 1, n, f); fclose(f);
        FILE *cf = fopen("/tmp/fmtl4-batch/cursor", "w"); fprintf(cf, "0"); fclose(cf);
        pid_t pid; char *av[5];
        av[0]=selfpath; av[1]=(char*)"--batch"; av[2]=(char*)dir; av[3]=(char*)"1"; av[4]=NULL;
        posix_spawn(&pid, selfpath, NULL, NULL, av, environ);
        int st; waitpid(pid, &st, 0);
        int rc = WIFEXITED(st) ? WEXITSTATUS(st) : 128+WTERMSIG(st);
        fprintf(stderr, "контроль: rc=%d (%s)\n", rc, rc==0?"OK":"FAIL");
        if (rc != 0 && rc != 1) return 3;
    }

    const int BATCH = 20;
    unsigned long total = 0, ok = 0, err = 0, crashes = 0, blacksz = 0;
    uint8_t *work = malloc(n);
    while (total < iters && crashes < 200) {
        // пачка
        int made = 0;
        unsigned long mut_offs[BATCH][8]; int mut_cnt[BATCH];
        for (int i = 0; i < BATCH && total < iters; i++, total++) {
            memcpy(work, seed, n);
            // чистый мутатор с трекингом оффсетов
            int rounds = 1 + (xr() % 6), cnt = 0;
            for (int r = 0; r < rounds; r++) {
                size_t off = xr() % n;
                if (black[off]) { r--; continue; }
                if (cnt < 8) mut_offs[i][cnt] = (unsigned long)off;
                cnt++;
                switch (xr() % 4) {
                case 0: work[off] ^= (uint8_t)(1u << (xr() % 8)); break;
                case 1: work[off] = (uint8_t)(xr() & 0xff); break;
                case 2: work[off] = 0; break;
                case 3: work[off] = 0xff; break;
                }
            }
            mut_cnt[i] = cnt;
            char vp[600]; snprintf(vp, sizeof(vp), "%s/variant%03d.bin", dir, i);
            FILE *f = fopen(vp, "wb"); fwrite(work, 1, n, f); fclose(f);
            made++;
        }
        FILE *cf = fopen("/tmp/fmtl4-batch/cursor", "w"); fprintf(cf, "0"); fclose(cf);
        char num[16]; snprintf(num, sizeof(num), "%d", made);
        pid_t pid; char *av[5];
        av[0]=selfpath; av[1]=(char*)"--batch"; av[2]=(char*)dir; av[3]=num; av[4]=NULL;
        if (posix_spawn(&pid, selfpath, NULL, NULL, av, environ) != 0) break;
        int st; waitpid(pid, &st, 0);
        int cursor = 0;
        cf = fopen("/tmp/fmtl4-batch/cursor", "r");
        if (cf) { if (fscanf(cf, "%d", &cursor) != 1) cursor = made; fclose(cf); }
        if (WIFEXITED(st) && WEXITSTATUS(st) == 0) {
            ok += made; err += 0;
        } else if (WIFEXITED(st) && WEXITSTATUS(st) == 1) {
            // все rejected? нет: cursor указывает где остановились...
            ok += cursor; err += (made - cursor);
        } else if (WIFSIGNALED(st)) {
            int cidx = cursor;   // краш на variant cidx
            crashes++;
            // blacklist оффсетов этого варианта
            if (cidx < made && cidx >= 0) {
                for (int k = 0; k < mut_cnt[cidx] && k < 8; k++) {
                    unsigned long off = mut_offs[cidx][k];
                    if (off < n) { black[off] = 1; blacksz++; }
                }
                // сохранить краш-вариант
                char name[128];
                snprintf(name, sizeof(name), "/tmp/fmtl4-crash-%lu.bin", total);
                char vp[600]; snprintf(vp, sizeof(vp), "%s/variant%03d.bin", dir, cidx);
                FILE *src = fopen(vp, "rb"); FILE *dst = fopen(name, "wb");
                if (src && dst) { char b[65536]; size_t r;
                    while ((r = fread(b,1,sizeof(b),src))>0) fwrite(b,1,r,dst);
                    fclose(src); fclose(dst);
                }
                fprintf(stderr, "CRASH iter=%lu variant=%d sig=%d saved (blacklist=%lu)\n",
                        total, cidx, WTERMSIG(st), blacksz);
            }
            ok += cidx;
        } else {
            ok += cursor;
        }
        if ((total % 2000) < BATCH)
            fprintf(stderr, "[%lu] ok=%lu err=%lu crash=%lu black=%lu\n",
                    total, ok, err, crashes, blacksz);
    }
    printf("done: %lu iters, ok=%lu err=%lu crash=%lu black=%lu\n",
           total, ok, err, crashes, blacksz);
    return crashes ? 2 : 0;
}
