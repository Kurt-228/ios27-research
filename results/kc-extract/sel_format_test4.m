// Round 4: zero-input selectors to isolate dispatch vs handler rejection.
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <IOKit/IOKitLib.h>

extern IOReturn IOConnectCallMethod(mach_port_t connection, uint32_t selector,
    const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt,
    uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);

int main(void) {
    io_connect_t conn;
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    kern_return_t kro = IOServiceOpen(svc, mach_task_self(), 0, &conn);
    printf("open kr=%#x\n", kro);

    IOSurfaceRef s = IOSurfaceCreate((__bridge CFDictionaryRef)@{@"IOSurfaceWidth": @8, @"IOSurfaceHeight": @8, @"IOSurfaceBytesPerElement": @4});
    uint32_t id = IOSurfaceGetID(s);
    printf("id=%u\n", id);

    // sel13 get_limits: no input, 40-byte structOut
    uint8_t out[64]; size_t osz = sizeof(out);
    kern_return_t kr = IOConnectCallMethod(conn, 13, NULL, 0, NULL, 0, NULL, NULL, out, &osz);
    printf("sel13 get_limits        kr=%#x osz=%zu\n", kr, osz);

    // sel16 get_surface_use_count: scalarIn[0]=sid, scalarOut[0]
    uint64_t sc = id, sout; uint32_t soutcnt = 1;
    kr = IOConnectCallMethod(conn, 16, &sc, 1, NULL, 0, &sout, &soutcnt, NULL, NULL);
    printf("sel16 use_count         kr=%#x val=%llu\n", kr, sout);

    // sel32 get_graphics_comm_page: scalarOut[0]
    sout = 0; soutcnt = 1;
    kr = IOConnectCallMethod(conn, 32, NULL, 0, NULL, 0, &sout, &soutcnt, NULL, NULL);
    printf("sel32 comm_page         kr=%#x val=%#llx\n", kr, sout);

    // sel10 get_value minimal, output struct 512
    uint8_t inb[64] = {0}; *(uint32_t*)inb = id; strcpy((char*)inb + 12, "NoSuch");
    uint8_t o2[512]; size_t o2s = sizeof(o2);
    kr = IOConnectCallMethod(conn, 10, NULL, 0, inb, 12 + 7, NULL, NULL, o2, &o2s);
    printf("sel10 get NoSuch        kr=%#x osz=%zu\n", kr, o2s);

    // sel23 is_tiled: scalarIn sid, scalarOut
    sout = 0; soutcnt = 1;
    kr = IOConnectCallMethod(conn, 23, &sc, 1, NULL, 0, &sout, &soutcnt, NULL, NULL);
    printf("sel23 is_tiled          kr=%#x val=%llu\n", kr, sout);
    return 0;
}
