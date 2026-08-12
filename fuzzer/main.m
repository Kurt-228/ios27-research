#import <UIKit/UIKit.h>
#import "fuzz.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate
- (BOOL)application:(UIApplication *)app didFinishLaunchingWithOptions:(NSDictionary *)opts {
    self.window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    UILabel *l = [[UILabel alloc] initWithFrame:self.window.bounds];
    l.text = @"fuzzing — see syslog";
    l.textAlignment = NSTextAlignmentCenter;
    UIViewController *vc = [UIViewController new];
    vc.view = l;
    self.window.rootViewController = vc;
    [self.window makeKeyAndVisible];

    const char *mode = getenv("FUZZ_MODE") ?: "all";
    LOG("[main] fuzz harness up, mode=%s", mode);
    LOG("[main] sandbox note: VCPDRM open is MACF-denied (iokit-open-user-client VCPDRMUserClient); "
        "mach-lookup com.apple.sprr / com.apple.jitbox exist but are sandbox-gated (seen in kernel log)");
    pthread_t t1, t2, t3;
    if (strstr(mode, "vcpdrm") || strstr(mode, "all"))
        pthread_create(&t1, NULL, t_vcpdrm, NULL);
    if (strstr(mode, "scaler") || strstr(mode, "all"))
        pthread_create(&t2, NULL, t_iosurface_scaler, NULL);
    if (strstr(mode, "mig") || strstr(mode, "all"))
        pthread_create(&t3, NULL, t_migscan, NULL);
    return YES;
}
@end

int main(int argc, char *argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass(AppDelegate.class));
    }
}
