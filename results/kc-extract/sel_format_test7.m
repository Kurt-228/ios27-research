// Find surface ID offset in lock result, then re-run set/get.
#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFSerialize.h>

extern IOReturn IOConnectCallMethod(mach_port_t connection, uint32_t selector,
    const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt,
    uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);

int main(void) {
    io_connect_t conn;
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    IOServiceOpen(svc, mach_task_self(), 0, &conn);

    NSDictionary *props = @{@"IOSurfaceWidth": @8, @"IOSurfaceHeight": @8, @"IOSurfaceBytesPerElement": @4};
    NSData *pb = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)props, kIOCFSerializeToBinary));
    uint64_t sc = 0;
    uint8_t out[3176] = {0}; size_t os = sizeof(out);
    kern_return_t kr = IOConnectCallMethod(conn, 0, &sc, 1, pb.bytes, pb.length, NULL, NULL, out, &os);
    printf("create kr=%#x\n", kr);
    printf("first 64 bytes: ");
    for (int i = 0; i < 64; i++) printf("%02x", out[i]);
    printf("\n");

    // try each u32 in first 64 bytes as sid against sel23 is_tiled (scalarIn sid, scalarOut)
    for (int off = 0; off < 64; off += 4) {
        uint32_t cand = *(uint32_t*)(out + off);
        if (cand == 0 || cand > 0x100000) continue;
        uint64_t si = cand, so = 0; uint32_t soc = 1;
        kern_return_t k2 = IOConnectCallMethod(conn, 23, &si, 1, NULL, 0, &so, &soc, NULL, NULL);
        printf("  off=%#04x cand=%u is_tiled kr=%#x\n", off, cand, k2);
    }
    return 0;
}
