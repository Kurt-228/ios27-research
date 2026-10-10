// iokitfuzz.c — v192: IOKit user-client sweep в VM-госте (root).
//
// Повод: в песочнице устройства открыты всего 5 из 460 сервисов
// (§142–145), здесь root открывает ВСЁ — тот же А17-прогон того же
// ядра, паника гостя бесплатна (перезагрузка по §196-пайплайну).
// Это прямая попытка добраться до IOKit-поверхности, недоступной
// on-device.
//
// Метод: рекурсивный обход IOService-плоскости -> дедуп по классу ->
// для каждого (класс, type 0..3) ДЕТЕРМИНИРОВАННО fork: ребёнок делает
// IOServiceOpen + перебор селекторов 0..0x1FF четырьмя формами ввода
// (пусто/0xff-скаляр/PRNG 256B/PRNG 4K) и _exit; родитель ждёт 60 с
// (WNOHANG) и бьёт тревогу IKF WEDGE при зависании. Зачем fork: в
// госте IOServiceOpen(AppleKeyStore) завис НАВСЕГДА (§192, qemu-окружение
// без keybag) — одна зависшая фаза раньше останавливала весь свип.
// Паника ядра убивает гость целиком — там fork не спасает, спасает
// точный постмортем: строка IKF TRY печатается ДО fork.
//
// Legacy IOConnectTrap0 в iOS-SDK недоступен — trap-путь не покрыт.
//
// Лог: HIT = kr==0 или неизвестный код (кандидат в «живые» селекторы);
// каждая попытка печатается ДО вызова (постмортем-репро). Аргумент 1:
// подстрока класса для включения либо список исключений с префиксом '!':
// '!Astris,!H1xANE' (§192 — оба дают воспроизводимый kernel data abort
// при IOServiceOpen; '!' допускается у КАЖДОГО паттерна):
//   iokitfuzz [<filter>|!<excl,...>] [<type>] [<sel0>] [<sel1>] [deep=1]
//
// Сборка:
//   xcrun -sdk iphoneos clang -target arm64-apple-ios17.0 \
//       vm/iokitfuzz.c -o iokitfuzz -framework IOKit && codesign -s - iokitfuzz
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <mach/mach.h>
#include <IOKit/IOKitLib.h>

static uint32_t g_seed = 0xC0FFEE;

static uint32_t rnd(void)
{
    g_seed = g_seed * 1664525u + 1013904223u;
    return g_seed;
}

// Мелкие штатные отказы user-client'ов — не «интересные».
static int known_err(kern_return_t kr)
{
    switch (kr) {
    case 0xe00002c2: /* bad argument       */
    case 0xe00002c1: /* not privileged     */
    case 0xe00002e2: /* not permitted      */
    case 0xe00002bc: /* unsupported        */
    case 0xe00002cd: /* no resources       */
    case 0xe00002c5: /* no space           */
    case 0xe00002c7: /* not ready          */
    case 0xe00002c9: /* not open?          */
    case 0xe00002ca: /* not readable       */
    case 0xe00002c6: /* vertical limit     */
    case KERN_INVALID_ARGUMENT:
    case KERN_FAILURE:
    case KERN_INVALID_RIGHT:
    case MACH_SEND_INVALID_DEST:
        return 1;
    default:
        return 0;
    }
}

// Ребёнок: open + селекторы. Всё печатает сам (наследует stdout-консоль).
static void do_service(io_service_t svc, const char *cls, int type,
                       int sel0, int sel1, int deep)
{
    mach_port_t conn = 0;
    kern_return_t kr = IOServiceOpen(svc, mach_task_self(), type, &conn);
    if (kr) {
        // v8: МАТРИЦА отказов — до v7 все неудачи были тихие (child
        // exit-0), OPEN=0 неотличим от «MACF deny» и «драйвер отказал».
        printf("IKF openerr %s t=%d kr=0x%x\n", cls, type, kr);
        _exit(0);
    }
    printf("IKF OPEN %s t=%d\n", cls, type);

    static unsigned char in[0x4000], out[0x4000];
    uint64_t sc_in[8] = {0}, sc_out[8] = {0};
    unsigned long long calls = 0;
    unsigned hits = 0, hitlog = 0;

    for (int sel = sel0; sel <= sel1; sel++) {
        int sh = (sel + type) & 3;
        size_t insz;
        uint32_t nsc;
        switch (sh) {
        case 0: /* скалярно, без структуры */
            insz = 0; nsc = 0;
            break;
        case 1: /* 0xff-скаляры + 8 байт 0xff */
            insz = 8; nsc = 4;
            memset(sc_in, 0xff, sizeof(sc_in));
            memset(in, 0xff, sizeof(in));
            break;
        case 2: /* PRNG 256B + два случайных скаляра */
            insz = 0x100; nsc = 2;
            sc_in[0] = rnd(); sc_in[1] = rnd();
            break;
        default: /* PRNG 4K */
            insz = 0x1000; nsc = 2;
            sc_in[0] = rnd(); sc_in[1] = rnd();
            break;
        }
        for (size_t i = 0; i < insz; i++) in[i] = (unsigned char)rnd();
        uint32_t nso = 8;
        size_t outs = sizeof(out);
        memset(out, 0xaa, sizeof(out));

        printf("IKF sel %s t=%d sel=0x%x sh=%d in=%zu\n",
               cls, type, sel, sh, insz); /* ДО вызова — репро при панике */
        kr = IOConnectCallMethod(conn, (uint32_t)sel, sc_in, nsc,
                                 in, insz, sc_out, &nso, out, &outs);
        calls++;
        int hit = (kr == KERN_SUCCESS) || !known_err(kr);
        if (hit) { hits++; hitlog++; }
        if ((hit && hitlog <= 4096) || deep)
            printf("IKF HIT %s t=%d sel=0x%x sh=%d kr=0x%x osz=%zu nso=%u\n",
                   cls, type, sel, sh, kr, outs, nso);
    }
    printf("IKF childdone %s t=%d calls=%llu hits=%u\n",
           cls, type, calls, hits);
    IOServiceClose(conn);
    _exit(0);
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    const char *only = argc > 1 ? argv[1] : NULL; // фильтр/исключения
    int only_type = argc > 2 ? atoi(argv[2]) : -1;
    int sel0 = argc > 3 ? (int)strtol(argv[3], NULL, 0) : 0;
    int sel1 = argc > 4 ? (int)strtol(argv[4], NULL, 0) : 0x1ff;
    int deep = argc > 5 ? atoi(argv[5]) : 0; // 1 = лог каждого вызова

    printf("IKF START pid=%d only=%s t=%d sel=0x%x..0x%x deep=%d\n",
           getpid(), only ? only : "*", only_type, sel0, sel1, deep);

    io_registry_entry_t root = IORegistryGetRootEntry(kIOMainPortDefault);
    if (!root) { printf("IKF CONTROL FAILED: no registry root\n"); return 1; }
    io_iterator_t it = 0;
    kern_return_t kr = IORegistryEntryCreateIterator(
        root, kIOServicePlane, kIORegistryIterateRecursively, &it);
    printf("IKF root=%u iterkr=0x%x\n", root, kr);
    if (kr) return 1;

    static char seen[900][96];
    int nseen = 0, nok = 0, nwedge = 0;
    io_service_t svc;

    while ((svc = IOIteratorNext(it))) {
        char cls[96] = {0};
        IOObjectGetClass(svc, cls);
        int dup = 0;
        for (int i = 0; i < nseen; i++)
            if (!strcmp(seen[i], cls)) { dup = 1; break; }
        if (dup || nseen >= 900) { IOObjectRelease(svc); continue; }
        if (only) {
            if (only[0] == '!' || only[0] == '-') {
                // список исключений через запятую, любой паттерн = skip;
                // у каждого паттерна может быть СВОЙ '!':
                // '!Astris,!H1xANE' — literal "!H1xANE" не найдётся
                int skip = 0;
                const char *p = only + 1;
                while (*p) {
                    const char *c = strchr(p, ',');
                    size_t len = c ? (size_t)(c - p) : strlen(p);
                    char pat[64];
                    if (len >= sizeof(pat)) len = sizeof(pat) - 1;
                    memcpy(pat, p, len);
                    pat[len] = 0;
                    const char *q = pat;
                    if (*q == '!' || *q == '-') q++;
                    if (*q && strstr(cls, q)) { skip = 1; break; }
                    p = c ? c + 1 : p + len;
                }
                if (skip) { IOObjectRelease(svc); continue; }
            } else if (!strstr(cls, only)) {
                IOObjectRelease(svc);
                continue;
            }
        }
        snprintf(seen[nseen], sizeof(seen[0]), "%s", cls);
        nseen++;

        for (int type = 0; type < 4; type++) {
            if (only_type >= 0 && type != only_type) continue;
            printf("IKF TRY %s t=%d\n", cls, type); /* ДО fork — репро */

            pid_t pid = fork();
            if (pid == 0) do_service(svc, cls, type, sel0, sel1, deep);
            if (pid < 0) { printf("IKF fork fail %s\n", cls); continue; }

            int status = 0, reaped = 0;
            for (int tick = 0; tick < 600; tick++) { /* 60 с на (класс,type) */
                if (waitpid(pid, &status, WNOHANG) == pid) { reaped = 1; break; }
                usleep(100000);
            }
            if (reaped) {
                nok++;
            } else {
                nwedge++;
                printf("IKF WEDGE %s t=%d — killing child\n", cls, type);
                kill(pid, SIGKILL);
                for (int tick = 0; tick < 50; tick++) {
                    if (waitpid(pid, &status, WNOHANG) == pid) break;
                    usleep(100000);
                }
            }
        }
        IOObjectRelease(svc);
    }

    printf("IKF DONE classes=%d ok=%d wedges=%d\n", nseen, nok, nwedge);
    return 0;
}
