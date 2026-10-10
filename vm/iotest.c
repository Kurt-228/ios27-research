// iotest.c — v192: проверка, что гость грузит IOKit-фреймворк из ramdisk
// (бинарь лежит по точному install-path /System/Library/Frameworks/
// IOKit.framework/Versions/A/IOKit, все зависимости на месте) и что MIG
// IORegistry работает. Это ворота для iokitfuzz.c — если dyld отвергает
// библиотеку, в serial-консоли будет его диагностика, а не тихий молк.
//
// Сборка:
//   xcrun -sdk iphoneos clang -target arm64-apple-ios17.0 \
//       vm/iotest.c -o iotest -framework IOKit && codesign -s - iotest
// Положить: ~/darwin-vm/addprog.sh iotest  (или в /private/var/tmp без sudo)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <IOKit/IOKitLib.h>

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("IOT pid=%d — IOKit framework load test\n", getpid());

    io_registry_entry_t root = IORegistryGetRootEntry(kIOMainPortDefault);
    printf("IOT root=%u\n", root);
    if (!root) {
        printf("IOT CONTROL FAILED: no registry root\n");
        return 1;
    }
    io_iterator_t it = 0;
    kern_return_t kr = IORegistryEntryCreateIterator(
        root, kIOServicePlane, kIORegistryIterateRecursively, &it);
    printf("IOT iterkr=0x%x\n", kr);
    if (kr) return 1;

    int n = 0, shown = 0;
    io_service_t svc;
    while ((svc = IOIteratorNext(it))) {
        char name[128] = {0};
        IORegistryEntryGetName(svc, name);
        if (shown < 5) printf("IOT svc[%d]=%s\n", n, name);
        n++;
        shown++;
        IOObjectRelease(svc);
    }
    printf("IOT DONE services=%d\n", n);
    return 0;
}
