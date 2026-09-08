// Round 5: which serialization does the kernel unserializer accept for sel0 props?
#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import <IOKit/IOCFSerialize.h>

extern IOReturn IOConnectCallMethod(mach_port_t connection, uint32_t selector,
    const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt,
    uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);

static io_connect_t gConn;
static void try0(const char *tag, NSData *props) {
    uint64_t sc = 0;
    uint8_t out[3176] = {0}; size_t os = sizeof(out);
    kern_return_t kr = IOConnectCallMethod(gConn, 0, &sc, 1, props.bytes, props.length, NULL, NULL, out, &os);
    printf("sel0 %-22s kr=%#x osz=%zu sids=%u,%u\n", tag, kr, os, *(uint32_t*)out, os>=8?*(uint32_t*)(out+4):0);
}

int main(void) {
    io_service_t svc = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOSurfaceRoot"));
    IOServiceOpen(svc, mach_task_self(), 0, &gConn);
    NSDictionary *props = @{@"IOSurfaceWidth": @8, @"IOSurfaceHeight": @8, @"IOSurfaceBytesPerElement": @4};
    try0("xml",  CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)props, 0)));
    try0("binary", CFBridgingRelease(IOCFSerialize((__bridge CFTypeRef)props, kIOCFSerializeToBinary)));
    NSString *xml = [NSString stringWithFormat:
        @"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"
        "<plist version=\"1.0\"><dict><key>IOSurfaceWidth</key><integer>8</integer>"
        "<key>IOSurfaceHeight</key><integer>8</integer>"
        "<key>IOSurfaceBytesPerElement</key><integer>4</integer></dict></plist>"];
    try0("raw-xml-string", [xml dataUsingEncoding:NSUTF8StringEncoding]);
    return 0;
}
