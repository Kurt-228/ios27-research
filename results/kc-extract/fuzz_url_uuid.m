// fuzz_url_uuid.m — фаззинг NSURL/CFURL и NSUUID (конвертеры ORM type-3/4/5).
// Эти парсеры выполнит appstored (root) на устройстве при чтении URL/UUID-колонок
// (app_install.artwork_url/bundle_url/... coordinator_id UUID, ...).
// Сборка: cc -arch arm64e -O2 -framework Foundation fuzz_url_uuid.m -o /tmp/fuu
#import <Foundation/Foundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

static unsigned long total = 0, oks = 0, errs = 0;
static sigjmp_buf jb;
static volatile sig_atomic_t hit;
static void crash_handler(int sig) { hit = sig; siglongjmp(jb, 1); }

static uint32_t rs;
static uint32_t xr(void) {
    rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs;
}

static void mutate(uint8_t *buf, size_t n) {
    int rounds = 1 + (xr() % 5);
    for (int r = 0; r < rounds; r++) {
        size_t off = xr() % n;
        static const char *urlchars =
            ":/%?#[]@!$&'()*+,;= %-_.~0123456789abcxyzABC"
            "\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80\\\"\t\r\n";
        switch (xr() % 5) {
        case 0: buf[off] ^= (uint8_t)(1u << (xr() % 8)); break;
        case 1: buf[off] = urlchars[xr() % strlen(urlchars)]; break;
        case 2: buf[off] = (uint8_t)(xr() & 0xff); break;
        case 3: buf[off] = "%\0"[xr() % 3]; break;
        case 4: buf[off] = (uint8_t)(0x80 + (xr() % 0x80)); break;
        }
    }
}

int main(int argc, char **argv) {
    unsigned long iters = argc > 1 ? strtoul(argv[1], NULL, 0) : 20000000;
    rs = argc > 2 ? (uint32_t)strtoul(argv[2], NULL, 0) : (uint32_t)getpid();
    NSArray *seeds = @[
        @"https://user:pw@[2001:db8::1]:8080/a/b/c%41?q=1&r=%zz#frag",
        @"itms-apps://apps.apple.com/app/id123?mt=8&uo=4",
        @"file:///var/containers/Bundle/Application/xyz/App.app/icon.png",
        @" App Store%3A//\xd0\xb8\xd0\xbc\xd1\x8f.com/\xf0\x9f\x98\x80",
        @"https://[::ffff:127.0.0.1]:65535/../../%2e%2e/%2f?#[",
        @"com.apple.appstored://callback?token=\xe2\x80\xae&x=1",
        @"http://a*b.cd/?a=b=c%#",
        @"6F9619FF-8B86-D011-B42D-00C04FC964FF",
        @"urn:uuid:12345678-1234-1234-1234-123456789012",
        @"/relative/path/../to/%00%ffnul",
    ];
    signal(SIGSEGV, crash_handler); signal(SIGBUS, crash_handler);
    signal(SIGABRT, crash_handler); signal(SIGILL, crash_handler);

    @autoreleasepool {
        for (unsigned long it = 0; it < iters; it++) {
            total++;
            NSString *seed = seeds[xr() % seeds.count];
            uint8_t buf[2048];
            NSData *d = [seed dataUsingEncoding:NSUTF8StringEncoding];
            size_t n = MIN(d.length, sizeof(buf) - 1);
            memcpy(buf, d.bytes, n);
            // иногда конкатенация двух сидов
            if ((xr() & 15) == 0) {
                NSString *s2 = seeds[xr() % seeds.count];
                NSData *d2 = [s2 dataUsingEncoding:NSUTF8StringEncoding];
                size_t n2 = MIN(d2.length, sizeof(buf) - 1 - n);
                memcpy(buf + n, d2.bytes, n2); n += n2;
            }
            mutate(buf, n);
            NSData *input = [NSData dataWithBytes:buf length:n];
            if (sigsetjmp(jb, 1) == 0) {
                // URL-путь (type-3): парсинг + норм-операции
                NSString *s = [[NSString alloc] initWithData:input
                        encoding:NSUTF8StringEncoding];
                if (s) {
                    NSURL *u = [NSURL URLWithString:s];
                    if (u) {
                        oks++;
                        [u absoluteString]; [u host]; [u port];
                        [u path]; [u query]; [u fragment]; [u user];
                        [u scheme]; [u resourceSpecifier];
                        [u URLByAppendingPathComponent:@"x/../y%20z"];
                        [u URLByStandardizingPath];
                        NSURL *r = [u URLByResolvingSymlinksInPath];
                        [u isEqual:r];
                    } else errs++;
                }
                // UUID-путь (type-4/5): 16-байтный blob + строковая форма
                NSUUID *u1 = [[NSUUID alloc] initWithUUIDString:s ?: @""];
                if (u1) { [u1 UUIDString]; [u1 hash]; }
                if (n >= 16) {
                    // конвертер type-5: UUID из 16 байт блоба — прямое CFUUID:
                    // эмулируем через NSUUID(data:) нет; используем CFUUIDCreateFromUUIDBytes
                    // через восстановление строки:
                    uint8_t ub[16]; memcpy(ub, buf, 16);
                    char str[40];
                    snprintf(str, sizeof(str),
                        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                        ub[0],ub[1],ub[2],ub[3],ub[4],ub[5],ub[6],ub[7],
                        ub[8],ub[9],ub[10],ub[11],ub[12],ub[13],ub[14],ub[15]);
                    NSUUID *u2 = [[NSUUID alloc] initWithUUIDString:
                        [NSString stringWithUTF8String:str]];
                    [u2 UUIDString];
                }
            } else {
                char name[128];
                snprintf(name, sizeof(name), "/tmp/fuu-crash-%lu.bin", total);
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
