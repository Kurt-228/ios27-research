#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFSerialize.h>
extern IOReturn IOConnectCallMethod(mach_port_t, uint32_t, const uint64_t*, uint32_t,
    const void*, size_t, uint64_t*, uint32_t*, void*, size_t*);
int main(void) {
    io_connect_t conn;
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    IOServiceOpen(svc, mach_task_self(), 0, &conn);
    NSDictionary *props = @{@"IOSurfaceWidth": @8, @"IOSurfaceHeight": @8, @"IOSurfaceBytesPerElement": @4};
    NSData *pb = CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)props, kIOCFSerializeToBinary));
    uint64_t sc = 0; uint8_t lr[3176] = {0}; size_t ls = sizeof(lr);
    IOConnectCallMethod(conn, 0, &sc, 1, pb.bytes, pb.length, NULL, NULL, lr, &ls);
    uint32_t id = *(uint32_t*)(lr + 0x18);
    printf("sid=%u\n", id);

    // get output header dump (value set via sel9 array)
    uint8_t setb[64] = {0}; *(uint32_t*)setb = id;
    NSData *arr = CFBridgingRelease(IOCFSerialize(@[@99, @"KA"], kIOCFSerializeToBinary));
    memcpy(setb+12, arr.bytes, arr.length);
    uint32_t tok; size_t tks=4;
    kern_return_t k1 = IOConnectCallMethod(conn, 9, NULL, 0, setb, 12+arr.length, NULL, NULL, &tok, &tks);
    printf("sel9 arr kr=%#x\n", k1);
    uint8_t inb[64] = {0}; *(uint32_t*)inb = id; strcpy((char*)inb+12, "KA");
    uint8_t o2[512] = {0}; size_t o2s = sizeof(o2);
    k1 = IOConnectCallMethod(conn, 10, NULL, 0, inb, 12+3, NULL, NULL, o2, &o2s);
    printf("sel10 KA kr=%#x osz=%zu hdr=%08x %08x %08x payload=", k1, o2s,
        *(uint32_t*)o2, *(uint32_t*)(o2+4), *(uint32_t*)(o2+8));
    for (size_t i=12;i<o2s;i++) printf("%02x", o2[i]); printf("\n");

    // sel27/28 roundtrip
    uint8_t b[160] = {0};
    for (int i=0;i<0x20;i++) b[i] = 0x40 + i;      // bit0: 32B
    *(uint64_t*)(b+0x30) = 0xdeadbeefcafebabeULL;  // bit2
    b[0x38] = 0x7f;                                // bit3
    b[0x39] = 0x55;                                // bit4
    *(uint64_t*)(b+0x90) = 0x7;                    // mask bits 0,1,2 (bit1 = +0x20 16B zeros)
    *(uint32_t*)(b+0x98) = id;
    k1 = IOConnectCallMethod(conn, 27, NULL, 0, b, 160, NULL, NULL, NULL, NULL);
    printf("sel27 mask=7 kr=%#x\n", k1);
    b[0x90] = 0xff; b[0x91]=0xff; b[0x92]=0xff; b[0x93]=0xff; b[0x94]=0xff;
    k1 = IOConnectCallMethod(conn, 27, NULL, 0, b, 160, NULL, NULL, NULL, NULL);
    printf("sel27 mask=bits0..39 kr=%#x\n", k1);
    k1 = IOConnectCallMethod(conn, 27, NULL, 0, b, 159, NULL, NULL, NULL, NULL);
    printf("sel27 size=159 kr=%#x (dispatch wants 160)\n", k1);
    // bad sid
    *(uint32_t*)(b+0x98) = 999999;
    k1 = IOConnectCallMethod(conn, 27, NULL, 0, b, 160, NULL, NULL, NULL, NULL);
    printf("sel27 bad sid kr=%#x\n", k1);
    *(uint32_t*)(b+0x98) = id;
    // sel28 get back
    uint64_t si = id; uint8_t g[160] = {0}; size_t gs = sizeof(g);
    k1 = IOConnectCallMethod(conn, 28, &si, 1, NULL, 0, NULL, NULL, g, &gs);
    printf("sel28 kr=%#x osz=%zu\n  [0..0x20]: ", k1, gs);
    for (int i=0;i<0x20;i++) printf("%02x", g[i]);
    printf("\n  [0x30..0x40]: ");
    for (int i=0x30;i<0x40;i++) printf("%02x", g[i]);
    printf(" [0x38]=%02x [0x39]=%02x\n", g[0x38], g[0x39]);

    // sel7 client_mem: own buffer
    void *mb = NULL; posix_memalign(&mb, 0x1000, 0x1000); memset(mb, 0x41, 0x1000);
    uint64_t mc[2] = {(uint64_t)mb, 0x1000};
    uint8_t mlr[3176] = {0}; size_t mls = sizeof(mlr);
    k1 = IOConnectCallMethod(conn, 7, mc, 2, NULL, 0, NULL, NULL, mlr, &mls);
    printf("sel7 client_mem kr=%#x osz=%zu newsid=%u\n", k1, mls, *(uint32_t*)(mlr+0x18));
    mc[0] = 1;
    k1 = IOConnectCallMethod(conn, 7, mc, 2, NULL, 0, NULL, NULL, mlr, &mls);
    printf("sel7 addr=1 kr=%#x\n", k1);
    mc[0] = (uint64_t)mb; mc[1] = 0;
    k1 = IOConnectCallMethod(conn, 7, mc, 2, NULL, 0, NULL, NULL, mlr, &mls);
    printf("sel7 size=0 kr=%#x\n", k1);
    return 0;
}
