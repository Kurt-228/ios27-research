// V156: the "victim" half of the cross-process experiment, as an XPC service.
//
// v155 proved, from inside one process, both necessary conditions for a
// zero-copy cross-process primitive:
//   1. sel7 creates a surface whose pages ARE the caller's memory
//      (base == our buffer, 100% marker bytes);
//   2. the sid keeps working after sel1 release — it lives in the global
//      IOSurfaceRoot registry, not bound to our client's references.
// What that does NOT prove is that a different TASK can use the sid, or that
// the surface survives that task's death. Both need a second process.
//
// This is that second process. posix_spawn of a Mach-O from inside the bundle
// is refused by the iOS sandbox (measured: errno 1 = EPERM), so the companion
// is an XPC service instead: launchd starts it on demand, it runs under its own
// pid/task, and the app talks to it over a real mach connection.
//
// Roles:
//   this service ("victim"): mint a client-memory surface over ITS OWN pages,
//       keep the sid, answer queries, and die on command. Dying is the point —
//       if the app can still read the surface afterwards, the descriptor
//       outlived its creator and its pages went back to the general allocator.
//   the app ("observer"): calls make, reads the surface back through its own
//       IOSurfaceRoot connection (control), then kills this service and reads
//       it again.
//
// The service holds no state shared with the app beyond the returned sid, so
// any positive result is attributable to the kernel's registry, not to shared
// memory in one address space.
#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>

@protocol VicProtocol
- (void)pingWithReply:(void (^)(uint64_t token))reply;
- (void)makeSurfaceWithMarker:(uint8_t)marker reply:(void (^)(int kr, uint32_t sid, uint64_t base))reply;
- (void)readSelfWithReply:(void (^)(int kr, long marker, long total))reply;
- (void)dieNowWithReply:(void (^)(void))reply;
@end

static void vlog(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    fprintf(stderr, "[vic] "); vfprintf(stderr, fmt, ap); fputc('\n', stderr);
    fflush(stderr); va_end(ap);
}

// Remembered so readSelf can verify from the INSIDE that our pages still hold
// the marker — that is the control for the observer's post-mortem read.
static uint8_t *g_buf; static size_t g_size; static uint8_t g_marker;

static io_connect_t open_surfaceroot(void) {
    io_service_t s = IOServiceGetMatchingService(kIOMainPortDefault,
                        IOServiceMatching("IOSurfaceRoot"));
    if (!s) return 0;
    io_connect_t c = 0;
    kern_return_t kr = IOServiceOpen(s, mach_task_self(), 0, &c);
    IOObjectRelease(s);
    return kr ? 0 : c;
}

@interface VicService : NSObject <VicProtocol, NSXPCListenerDelegate> @end

@implementation VicService {
    io_connect_t _uc;
    uint32_t _sid;
}

- (instancetype)init {
    self = [super init];
    _uc = open_surfaceroot();
    _sid = 0;
    vlog("up, pid %d IOSurfaceRoot conn 0x%x", getpid(), _uc);
    return self;
}

- (void)pingWithReply:(void (^)(uint64_t))reply { reply(0xBEEF); }

- (void)makeSurfaceWithMarker:(uint8_t)marker
                        reply:(void (^)(int, uint32_t, uint64_t))reply {
    g_size = 0x40000;
    vm_address_t a = 0;
    if (vm_allocate(mach_task_self(), &a, g_size, VM_FLAGS_ANYWHERE)) {
        vlog("vm_allocate failed"); reply(-1, 0, 0); return;
    }
    g_buf = (uint8_t *)a;
    g_marker = marker;
    for (size_t i = 0; i < g_size; i++) g_buf[i] = marker;

    uint8_t *outb = (uint8_t *)calloc(1, 0x2000);
    uint64_t sc[2] = { (uint64_t)(uintptr_t)g_buf, (uint64_t)g_size };
    uint64_t osc[4] = {0,0,0,0}; uint32_t nosc = 0; size_t osz = 3176;
    kern_return_t kr = IOConnectCallMethod(_uc, 7, sc, 2, NULL, 0,
                                           osc, &nosc, outb, &osz);
    _sid = *(uint32_t *)(outb + 0x18);
    free(outb);
    vlog("sel7 -> kr 0x%08x sid %u (buf %p size 0x%zx marker 0x%02x)",
         kr, _sid, g_buf, g_size, marker);
    reply((int)kr, _sid, (uint64_t)(uintptr_t)g_buf);
}

- (void)readSelfWithReply:(void (^)(int, long, long))reply {
    long mark = 0;
    if (g_buf) for (size_t i = 0; i < g_size; i++) if (g_buf[i] == g_marker) mark++;
    reply(0, mark, (long)g_size);
}

- (void)dieNowWithReply:(void (^)(void))reply {
    vlog("asked to die (pid %d) with sid %u still live", getpid(), _sid);
    reply();
    [self performSelector:@selector(exitSoon) withObject:nil afterDelay:1.0];
}

- (void)exitSoon {
    // _exit, not exit: we want the task gone without unwinding the alias
    // descriptor. The surface must outlive us for the test to mean anything.
    vlog("exiting now");
    _exit(0);
}

// The listener delegate is normally a separate object; keeping it on the same
// service instance lets the service read its own sid when a reply is built.
- (NSXPCConnection *)listener:(NSXPCListener *)l
    shouldAcceptNewConnection:(NSXPCConnection *)c {
    vlog("accepted XPC connection from pid %d", c.processIdentifier);
    c.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(VicProtocol)];
    c.exportedObject = self;
    c.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(VicProtocol)];
    [c resume];
    return c;
}
@end

int main(int argc, char **argv) {
    vlog("XPC service starting, argc %d", argc);
    VicService *svc = [VicService new];
    // Standard XPC service plumbing: NSXPCListener on our bundle id.
    NSXPCConnection *listener = [[NSXPCListener alloc]
        initWithMachServiceName:@"com.cancer9725.vic"];
    if (!listener) { vlog("initWithMachServiceName returned nil"); return 1; }
    listener.delegate = (id)svc;
    [listener resume];
    vlog("listener resumed on com.cancer9725.vic");
    [[NSRunLoop currentRunLoop] run];
    return 0;
}