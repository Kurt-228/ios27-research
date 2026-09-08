// Round 3: correct structureOutput usage.
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFSerialize.h>

extern IOReturn IOConnectCallMethod(mach_port_t connection, uint32_t selector,
    const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt,
    uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);

static io_connect_t gConn;
static uint32_t gID;

static void test_set(const char *tag, NSData *blob) {
    size_t n = 12 + blob.length;
    uint8_t *buf = calloc(1, n);
    *(uint32_t*)buf = gID;
    memcpy(buf + 12, blob.bytes, blob.length);
    uint32_t token = 0xdeadbeef;
    size_t tokn = 4;
    kern_return_t kr = IOConnectCallMethod(gConn, 9, NULL, 0, buf, n, NULL, NULL, &token, &tokn);
    printf("sel9  %-32s kr=%#x token=%08x\n", tag, kr, token);
    free(buf);
}

static void test_get(const char *key) {
    uint8_t inb[64] = {0};
    *(uint32_t*)inb = gID;
    strcpy((char*)inb + 12, key);
    uint8_t out[512] = {0};
    size_t outsz = sizeof(out);
    kern_return_t kr = IOConnectCallMethod(gConn, 10, NULL, 0, inb, 12 + strlen(key) + 1,
                                           NULL, NULL, out, &outsz);
    printf("sel10 key=%-8s kr=%#x outsz=%zu data='%.*s'\n", key, kr, outsz,
           outsz > 12 ? (int)(outsz - 12) : 0, out + 12);
}

int main(void) {
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    IOServiceOpen(svc, mach_task_self(), 0, &gConn);

    NSDictionary *props = @{@"IOSurfaceWidth": @8, @"IOSurfaceHeight": @8, @"IOSurfaceBytesPerElement": @4};
    NSData *pb = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)props, kIOCFSerializeToBinary));
    uint8_t in0[256] = {0};
    memcpy(in0 + 12, pb.bytes, pb.length);
    uint64_t sc0 = 0;
    uint8_t out0[3176] = {0};
    size_t out0s = sizeof(out0);
    kern_return_t kr0 = IOConnectCallMethod(gConn, 0, &sc0, 1, in0, 12 + pb.length, NULL, NULL, out0, &out0s);
    printf("sel0 create kr=%#x outsz=%zu sid=%u\n", kr0, out0s, *(uint32_t*)out0);

    IOSurfaceRef s = IOSurfaceCreate((__bridge CFDictionaryRef)props);
    gID = IOSurfaceGetID(s);
    printf("surface id=%u\n", gID);

    test_get("NoSuch");

    NSArray *arrKV = @[@99, @"KA"];
    NSArray *arrVK = @[@"KB", @99];
    test_set("xml array [99,KA]",   CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrKV, 0)));
    test_set("bin  array [99,KA]",  CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrKV, kIOCFSerializeToBinary)));
    test_set("xml array [KB,99]",   CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrVK, 0)));
    test_set("bin  array [KB,99]",  CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrVK, kIOCFSerializeToBinary)));
    test_set("xml dict {K1:42}",    CFBridgingRelease(IOCFSerialize(@{@"K1": @42}, 0)));
    test_set("bin  dict {K1:42}",   CFBridgingRelease(IOCFSerialize(@{@"K1": @42}, kIOCFSerializeToBinary)));

    test_get("KA");
    test_get("KB");
    test_get("K1");
    return 0;
}
