// fuzz_nsjso.m — фаззинг NSJSONSerialization (+ NSKeyedUnarchiver) на Mac.
// Это тот же Foundation-код, что выполнится в appstored (root) на устройстве,
// когда ORM (ASUSQLiteCopyFoundationValue type-2) декодирует наши blob-байты.
// Оракул: SIGSEGV/SIGABRT + guard-страницы не нужны (парсер Foundation).
// Мутации: битфлипы + structured (глубина, unicode, числа, длинные строки).
// Сборка: cc -arch arm64e -framework Foundation -O2 fuzz_nsjso.m -o /tmp/fj
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

static unsigned long total = 0, errs = 0, oks = 0;
static sigjmp_buf jb;
static volatile sig_atomic_t hit;

static void crash_handler(int sig) {
    hit = 1;
    siglongjmp(jb, 1);
}

static uint32_t rnd_state;
static uint32_t xr(void) {
    rnd_state ^= rnd_state << 13; rnd_state ^= rnd_state >> 17;
    rnd_state ^= rnd_state << 5; return rnd_state;
}

// мутация одного байта блоба
static void mutate(uint8_t *buf, size_t n) {
    int rounds = 1 + (xr() % 4);
    for (int r = 0; r < rounds; r++) {
        size_t off = xr() % n;
        switch (xr() % 4) {
        case 0: buf[off] ^= (uint8_t)(1u << (xr() % 8)); break;
        case 1: buf[off] = (uint8_t)(xr() & 0xff); break;
        case 2: buf[off] = "[]{}\",:-0123456789.eE+-truefalsnl \t\n\\"[xr() % 30]; break;
        case 3: buf[off] = (uint8_t)(0x80 + (xr() % 0x80)); break;
        }
    }
}

int main(int argc, char **argv) {
    unsigned long iters = argc > 1 ? strtoul(argv[1], NULL, 0) : 20000000;
    rnd_state = argc > 2 ? (uint32_t)strtoul(argv[2], NULL, 0) : (uint32_t)getpid();

    // сиды: валидные JSON-структуры (включая глубокие)
    NSArray *seeds = @[
        @"{\"a\":[1,2,{\"b\":\"c\"}],\"d\":true,\"e\":null}",
        @"[[[[[[[[[[1]]]]]]]]]]",
        @"{\"metrics\":{\"fields\":[{\"k\":\"v\",\"n\":-1.5e300}]}",
        @"[{\"policy\":{\"entitlements\":[\"a\",\"b\"],\"count\":18446744073709551615}}]",
        @"{\"deep\":0,\"nested\":{\"x\":{\"y\":{\"z\":[0,1,2,3,{\"w\":\"\\u00e9\\ud83d\\ude00\"}]}}}}",
        @"[3.14159265358979,1e308,-1e-308,0.1,1.7976931348623157e308]",
        @"\"just a plain string\"",
        @"{\"data_promise\":{\"uuid\":\"6F9619FF-8B86-D011-B42D-00C04FC964FF\"}}",
    ];
    signal(SIGSEGV, crash_handler);
    signal(SIGBUS, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGILL, crash_handler);

    @autoreleasepool {
        for (unsigned long it = 0; it < iters; it++) {
            total++;
            NSString *seed = seeds[xr() % seeds.count];
            // иногда углубляем/растягиваем сид
            if ((xr() & 7) == 0) {
                NSMutableString *ms = [seed mutableCopy];
                for (int i = 0, n = 1 + (xr() % 40); i < n; i++)
                    [ms insertString:@"[[[[" atIndex:0];
                seed = ms;
            }
            NSData *d = [seed dataUsingEncoding:NSUTF8StringEncoding];
            uint8_t buf[8192];
            size_t n = MIN(d.length, sizeof(buf) - 1);
            memcpy(buf, d.bytes, n);
            mutate(buf, n);
            NSData *input = [NSData dataWithBytes:buf length:n];

            if (sigsetjmp(jb, 1) == 0) {
                NSError *err = nil;
                id obj = [NSJSONSerialization JSONObjectWithData:input
                    options:(NSJSONReadingMutableContainers|NSJSONReadingAllowFragments)
                    error:&err];
                if (obj) oks++; else errs++;
                if (obj) { // round-trip stress
                    if ([NSJSONSerialization isValidJSONObject:obj]) {
                        [NSJSONSerialization dataWithJSONObject:obj
                            options:0 error:nil];
                    }
                }
            } else {
                char name[128];
                snprintf(name, sizeof(name), "/tmp/fjso-crash-%lu.bin", total);
                FILE *f = fopen(name, "wb");
                if (f) { fwrite(buf, 1, n, f); fclose(f); }
                fprintf(stderr, "CRASH iter=%lu sig=%d saved=%s\n", total, hit, name);
                hit = 0;
            }
            if ((total % 1000000) == 0)
                fprintf(stderr, "[%lu] ok=%lu err=%lu\n", total, oks, errs);
        }
    }
    printf("done: %lu iters, ok=%lu err=%lu\n", total, oks, errs);
    return 0;
}
