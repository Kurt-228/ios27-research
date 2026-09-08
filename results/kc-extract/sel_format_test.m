// Ground-truth test for IOSurfaceRootUserClient selector formats (macOS 27).
// Verifies RE-derived formats for sel 9/10 (set/get value) and sel 27/28 (bulk attachments).
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFSerialize.h>

extern IOReturn IOConnectCallMethod(mach_port_t connection, uint32_t selector,
    const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt,
    uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);

static io_connect_t gConn;

static void open_root(void) {
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    assert(svc);
    kern_return_t kr = IOServiceOpen(svc, mach_task_self(), 0, &gConn);
    printf("IOServiceOpen(IOSurfaceRoot) kr=%#x conn=%d\n", kr, gConn);
    assert(kr == 0);
}

static uint32_t sid(void) {
    static IOSurfaceRef s;
    if (!s) {
        NSDictionary *d = @{@"IOSurfaceWidth": @4, @"IOSurfaceHeight": @4,
                            @"IOSurfaceBytesPerElement": @4, @"IOSurfacePixelFormat": @'BGRA'};
        s = IOSurfaceCreate((__bridge CFDictionaryRef)d);
        assert(s);
    }
    return IOSurfaceGetID(s);
}

// sel9 set_value: struct = {u32 sid; u32 pad; u32 pad; serialized[]}
static void test_set(const char *tag, uint32_t surface_id, NSData *blob) {
    size_t n = 12 + blob.length;
    uint8_t *buf = calloc(1, n);
    *(uint32_t*)buf = surface_id;
    memcpy(buf + 12, blob.bytes, blob.length);
    kern_return_t kr = IOConnectCallMethod(gConn, 9, NULL, 0, buf, n, NULL, NULL, NULL, NULL);
    printf("sel9  %-28s kr=%#x (%d)\n", tag, kr, kr);
    free(buf);
}

// sel10 get_value: structIn = {u32 sid; u32 pad; u32 pad; key\0}, structOut buffer
static void test_get(const char *key) {
    uint8_t inb[64] = {0};
    *(uint32_t*)inb = sid();
    strcpy((char*)inb + 12, key);
    uint8_t outb[512] = {0};
    size_t outsz = sizeof(outb);
    kern_return_t kr = IOConnectCallMethod(gConn, 10, NULL, 0, inb, 12 + strlen(key) + 1,
                                           NULL, NULL, outb, &outsz);
    printf("sel10 key=%-8s kr=%#x outsz=%zu first32=%08x\n", key, kr, outsz, outsz ? *(uint32_t*)outb : 0);
    if (kr == 0 && outsz > 12)
        printf("      out bytes[12..]: %.*s\n", (int)MIN(outsz - 12, 80), outb + 12);
}

int main(void) {
    open_root();
    uint32_t id = sid();
    printf("surface id=%u\n", id);

    // A: binary IOCFSerialize of one-key dict
    NSDictionary *d1 = @{@"K1": @42};
    NSData *bin1 = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)d1, kIOCFSerializeToBinary));
    printf("bin1 len=%zu magic=%08x\n", bin1.length, bin1.length ? *(uint32_t*)bin1.bytes : 0);
    test_set("binary {K1:42}", id, bin1);

    // B: XML IOCFSerialize of one-key dict
    NSData *xml1 = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)d1, 0));
    test_set("xml {K1:42}", id, xml1);

    // C: binary of dict with TWO keys
    NSDictionary *d2 = @{@"First": @1, @"K2": @7};
    NSData *bin2 = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)d2, kIOCFSerializeToBinary));
    test_set("binary {First:1,K2:7}", id, bin2);

    // D: binary of ARRAY [value, key]
    NSArray *a1 = @[@99, @"KA"];
    NSData *bin3 = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)a1, kIOCFSerializeToBinary));
    test_set("binary [99,\"KA\"]", id, bin3);

    // E: {id,key,value}-style dict like the rejected iOS skeletons
    NSDictionary *d3 = @{@"id": @1, @"key": @"KB", @"value": @5};
    NSData *bin4 = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)d3, kIOCFSerializeToBinary));
    test_set("binary {id,key,value}", id, bin4);

    // read back
    test_get("K1");
    test_get("First");
    test_get("K2");
    test_get("KA");
    test_get("id");
    test_get("KB");

    // ---- sel27 bulk attachments ----
    // struct 160 bytes: payload to 0x83, mask u64 @0x90, sid u32 @0x98
    uint8_t b[160] = {0};
    *(uint64_t*)(b + 0x00) = 0x1122334455667788ULL; // bit0 field (32 bytes)
    *(uint64_t*)(b + 0x08) = 0x99aabbccddeeff00ULL;
    *(uint64_t*)(b + 0x10) = 0x0102030405060708ULL;
    *(uint64_t*)(b + 0x18) = 0x1112131415161718ULL;
    *(uint64_t*)(b + 0x30) = 0xdeadbeefcafebabeULL; // bit2 u64
    b[0x38] = 0x7f;                                 // bit3 u8
    *(uint64_t*)(b + 0x90) = 0x7;                   // mask bits 0,1,2
    *(uint32_t*)(b + 0x98) = id;                    // sid
    kern_return_t kr27 = IOConnectCallMethod(gConn, 27, NULL, 0, b, 160, NULL, NULL, NULL, NULL);
    printf("sel27 mask=7       kr=%#x\n", kr27);
    *(uint64_t*)(b + 0x90) = 0xffffffffffffffffULL;
    kr27 = IOConnectCallMethod(gConn, 27, NULL, 0, b, 160, NULL, NULL, NULL, NULL);
    printf("sel27 mask=all     kr=%#x\n", kr27);
    kr27 = IOConnectCallMethod(gConn, 27, NULL, 0, b, 159, NULL, NULL, NULL, NULL);
    printf("sel27 size=159     kr=%#x (want 1000000e)\n", kr27);

    // sel28 get_bulk_attachments: scalarIn[0]=sid, structOut 160
    uint64_t sc = id;
    uint8_t outb[160] = {0};
    size_t outsz = sizeof(outb);
    kern_return_t kr28 = IOConnectCallMethod(gConn, 28, &sc, 1, NULL, 0, NULL, NULL, outb, &outsz);
    printf("sel28 kr=%#x outsz=%zu\n", kr28, outsz);
    if (kr28 == 0) {
        printf("  [0..0x20]  : "); for (int i = 0; i < 0x20; i++) printf("%02x", outb[i]);
        printf("\n  [0x30..0x40]: "); for (int i = 0x30; i < 0x40; i++) printf("%02x", outb[i]);
        printf("\n  [0x38]=%02x\n", outb[0x38]);
    }
    return 0;
}
