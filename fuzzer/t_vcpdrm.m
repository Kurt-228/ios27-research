// Target: VCPDRMServiceUserClient (new kext in iOS 27.0, ~300 insn, 3 selectors)
// Map (from static analysis, doc sec.37):
//   dispatch: w1<=2, scalarInputCount==0, structureInput ptr at args+0x20 -> u64
//   handler A: rate-limited; input id in [1..0x20]; slots 0x30B each; stores objects + u64
//   handler B: unregister by id
//   RACE: slot table has no visible locks -> 2 threads hammer register/unregister same id
#include "fuzz.h"

static io_connect_t g_conn;

static void call_sel(uint32_t sel, uint64_t val, int tag) {
    uint64_t out = 0;
    size_t outsz = 8;
    kern_return_t kr = IOConnectCallMethod(g_conn, sel,
        NULL, 0,                      // scalar input: none (required!)
        &val, 8,                      // structure input: one u64
        NULL, NULL,
        &out, &outsz);
    if (kr && kr != 0xe00002c2 && kr != 0xe00002c9 && kr != 0xe00002bc && kr != 0xe00002f0)
        LOG("[vcpdrm t%d] sel %u val 0x%llx -> 0x%x", tag, sel, val, kr);
}

static void *race_reg(void *arg) {
    long id = (long)arg;
    for (;;) {
        call_sel(frand_range(0, 2), (uint64_t)id, 1);
        call_sel(frand_range(0, 2), (uint64_t)id, 1);
    }
    return NULL;
}
static void *race_unreg(void *arg) {
    long id = (long)arg;
    for (;;) {
        call_sel(frand_range(0, 2), (uint64_t)id, 2);
    }
    return NULL;
}

void *t_vcpdrm(void *arg) {
    g_conn = open_service("VCPDRMService", 0);
    if (!g_conn) g_conn = open_service("VCPDRMServiceUserClient", 0);
    if (!g_conn) { LOG("[vcpdrm] no connection, target dead"); return NULL; }

    // phase 1: sequential probe — walk selectors 0..3, ids and odd values
    for (int round = 0; round < 200000; round++) {
        uint32_t sel = (uint32_t)frand_range(0, 3);
        uint64_t v;
        switch (frand() % 6) {
            case 0: v = frand_range(1, 0x20); break;      // legal ids
            case 1: v = frand_range(0, 0x40); break;      // around edges
            case 2: v = frand_range(0, 0x100); break;
            case 3: v = frand(); break;                   // raw
            case 4: v = frand_range(1, 0x20) | (frand() << 32); break;
            default: v = 0; break;
        }
        call_sel(sel, v, 0);
        // occasionally hit rate limit (>301 calls/period) intentionally
        if ((round & 0x3ff) == 0) usleep(1000);
    }

    // phase 2: race — same id register/unregister from two threads, each selector pairing
    LOG("[vcpdrm] entering race phase");
    for (long id = 1; id <= 0x20; id++) {
        pthread_t a, b;
        pthread_create(&a, NULL, race_reg, (void *)id);
        pthread_create(&b, NULL, race_unreg, (void *)id);
        usleep(50000);
        pthread_cancel(a); pthread_cancel(b);
        pthread_join(a, NULL); pthread_join(b, NULL);
    }
    LOG("[vcpdrm] done (restart to continue)");
    return NULL;
}
