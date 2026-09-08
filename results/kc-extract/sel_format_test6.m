// Round 6: full format validation with own-connection surface.
#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFSerialize.h>

extern IOReturn IOConnectCallMethod(mach_port_t connection, uint32_t selector,
    const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt,
    uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);

static io_connect_t gConn;
static uint32_t gID;

static void create_surface(void) {
    NSDictionary *props = @{@"IOSurfaceWidth": @8, @"IOSurfaceHeight": @8, @"IOSurfaceBytesPerElement": @4};
    NSData *pb = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)props, kIOCFSerializeToBinary));
    uint64_t sc = 0;
    uint8_t out[3176] = {0}; size_t os = sizeof(out);
    kern_return_t kr = IOConnectCallMethod(gConn, 0, &sc, 1, pb.bytes, pb.length, NULL, NULL, out, &os);
    // find sid: scan output for plausible small u32 pair; IOSurfaceLockResult.surfaceID near start
    printf("sel0 kr=%#x osz=%zu u32[0..3]=%u %u %u %u\n", kr, os,
           *(uint32_t*)out, *(uint32_t*)(out+4), *(uint32_t*)(out+8), *(uint32_t*)(out+12));
    gID = *(uint32_t*)out; // hypothesis
    // sanity: is_tiled with this sid must succeed if sid correct
    uint64_t si = gID, so; uint32_t soc = 1;
    kern_return_t k2 = IOConnectCallMethod(gConn, 23, &si, 1, NULL, 0, &so, &soc, NULL, NULL);
    printf("  sid=%u is_tiled kr=%#x\n", gID, k2);
    if (k2 != 0) { // try offset 4
        gID = *(uint32_t*)(out+4);
        k2 = IOConnectCallMethod(gConn, 23, &(uint64_t){gID}, 1, NULL, 0, &so, &soc, NULL, NULL);
        printf("  sid=%u is_tiled kr=%#x\n", gID, k2);
    }
}

static void test_set(const char *tag, NSData *blob) {
    size_t n = 12 + blob.length;
    uint8_t *buf = calloc(1, n);
    *(uint32_t*)buf = gID;
    memcpy(buf + 12, blob.bytes, blob.length);
    uint32_t token = 0; size_t tk = 4;
    kern_return_t kr = IOConnectCallMethod(gConn, 9, NULL, 0, buf, n, NULL, NULL, &token, &tk);
    printf("sel9  %-26s kr=%#x token=%u\n", tag, kr, token);
    free(buf);
}

static void test_get(const char *key) {
    uint8_t inb[64] = {0};
    *(uint32_t*)inb = gID;
    strcpy((char*)inb + 12, key);
    uint8_t out[512] = {0}; size_t osz = sizeof(out);
    kern_return_t kr = IOConnectCallMethod(gConn, 10, NULL, 0, inb, 12 + strlen(key) + 1, NULL, NULL, out, &osz);
    printf("sel10 key=%-8s kr=%#x osz=%zu data='%.*s'\n", key, kr, osz, osz > 12 ? (int)(osz-12) : 0, out+12);
}

int main(void) {
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    IOServiceOpen(svc, mach_task_self(), 0, &gConn);
    create_surface();

    NSArray *arrKV = @[@99, @"KA"];
    test_set("bin array [99,KA]",  CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrKV, kIOCFSerializeToBinary)));
    test_set("xml array [99,KA]",  CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrKV, 0)));
    test_set("bin dict {K1:42}",   CFBridgingRelease(IOCFSerialize(@{@"K1": @42}, kIOCFSerializeToBinary)));
    test_set("xml dict {K1:42}",   CFBridgingRelease(IOCFSerialize(@{@"K1": @42}, 0)));
    test_set("bin dict {id,k,v}",  CFBridgingRelease(IOCFSerialize(@{@"id": @1, @"key": @"KB", @"value": @5}, kIOCFSerializeToBinary)));
    test_set("bin str \"S\"",      CFBridgingRelease(IOCFSerialize(@"S", kIOCFSerializeToBinary)));

    test_get("KA"); test_get("K1"); test_get("id"); test_get("key");
    return 0;
}
