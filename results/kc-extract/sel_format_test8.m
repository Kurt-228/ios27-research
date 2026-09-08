#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFSerialize.h>
extern IOReturn IOConnectCallMethod(mach_port_t, uint32_t, const uint64_t*, uint32_t,
    const void*, size_t, uint64_t*, uint32_t*, void*, size_t*);
static io_connect_t gConn; static uint32_t gID;
static void setup(void) {
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    IOServiceOpen(svc, mach_task_self(), 0, &gConn);
    NSDictionary *props = @{@"IOSurfaceWidth": @8, @"IOSurfaceHeight": @8, @"IOSurfaceBytesPerElement": @4};
    NSData *pb = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)props, kIOCFSerializeToBinary));
    uint64_t sc = 0; uint8_t out[3176] = {0}; size_t os = sizeof(out);
    IOConnectCallMethod(gConn, 0, &sc, 1, pb.bytes, pb.length, NULL, NULL, out, &os);
    gID = *(uint32_t*)(out + 0x18);
    printf("sid=%u\n", gID);
}
static void t_set(const char *tag, NSData *blob, uint32_t flags) {
    size_t n = 12 + blob.length;
    uint8_t *buf = calloc(1, n);
    *(uint32_t*)buf = gID; *(uint32_t*)(buf+8) = flags;
    memcpy(buf + 12, blob.bytes, blob.length);
    uint32_t token = 0; size_t tk = 4;
    kern_return_t kr = IOConnectCallMethod(gConn, 9, NULL, 0, buf, n, NULL, NULL, &token, &tk);
    printf("sel9  %-26s kr=%#x token=%u\n", tag, kr, token);
    free(buf);
}
static void t_get(const char *key, uint32_t flags) {
    uint8_t inb[64] = {0};
    *(uint32_t*)inb = gID; *(uint32_t*)(inb+8) = flags;
    strcpy((char*)inb + 12, key);
    uint8_t out[512] = {0}; size_t osz = sizeof(out);
    kern_return_t kr = IOConnectCallMethod(gConn, 10, NULL, 0, inb, 12 + strlen(key) + 1, NULL, NULL, out, &osz);
    printf("sel10 key=%-6s flags=%u kr=%#x osz=%zu data='%.*s'\n", key, flags, kr, osz, osz>12?(int)(osz-12):0, out+12);
}
int main(void) {
    setup();
    NSArray *arrKV = @[@99, @"KA"];
    t_set("bin array [99,KA]", CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrKV, kIOCFSerializeToBinary)), 0);
    t_set("xml array [99,KA]", CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)arrKV, 0)), 0);
    t_set("bin dict {K1:42}",  CFBridgingRelease(IOCFSerialize(@{@"K1": @42}, kIOCFSerializeToBinary)), 0);
    t_set("xml dict {K1:42}",  CFBridgingRelease(IOCFSerialize(@{@"K1": @42}, 0)), 0);
    t_set("bin dict {id,k,v}", CFBridgingRelease(IOCFSerialize(@{@"id":@1,@"key":@"KB",@"value":@5}, kIOCFSerializeToBinary)), 0);
    t_get("KA", 0); t_get("K1", 0); t_get("id", 0); t_get("key", 0); t_get("NoSuch", 0);
    t_get("KA", 1);
    return 0;
}
