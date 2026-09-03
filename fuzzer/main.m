#import <UIKit/UIKit.h>
#import "fuzz.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate
// iOS 27 hard-crashes apps without scene-lifecycle adoption at scene
// creation (SIGTRAP, __UIApplicationEvaluateRuntimeIssueForNoSceneLifecycleAdoption).
// Providing a scene configuration (even without a SceneDelegate) satisfies
// the check while the legacy app-delegate window path keeps working.
- (UISceneConfiguration *)application:(UIApplication *)app
    configurationForConnectingSceneSession:(UISceneSession *)session
                                   options:(UISceneConnectionOptions *)opts {
    return [[UISceneConfiguration alloc] initWithName:@"Default" sessionRole:session.role];
}
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
    // Fallback log path: when devicectl --console attach is broken, run with
    // FUZZ_LOGFILE=1 to tee stderr into the app container and pull it via
    // devicectl copy (domain-type appDataContainer).
    if (getenv("FUZZ_LOGFILE")) {
        NSString *logp = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents/fuzz.log"];
        freopen(logp.fileSystemRepresentation, "w", stderr);
        setvbuf(stderr, NULL, _IONBF, 0);
    }
    LOG("[main] fuzz harness up, mode=%s", mode);
    LOG("[main] sandbox note: VCPDRM open is MACF-denied (iokit-open-user-client VCPDRMUserClient); "
        "mach-lookup com.apple.sprr / com.apple.jitbox exist but are sandbox-gated (seen in kernel log)");
    pthread_t t1, t2, t3, t4;
    if (strstr(mode, "vcpdrm") || strstr(mode, "all"))
        pthread_create(&t1, NULL, t_vcpdrm, NULL);
    if (strstr(mode, "scaler") || strstr(mode, "all"))
        pthread_create(&t2, NULL, t_iosurface_scaler, NULL);
    if (strstr(mode, "mig") || strstr(mode, "all"))
        pthread_create(&t3, NULL, t_migscan, NULL);
    if (strstr(mode, "xpleak"))
        pthread_create(&t4, NULL, t_xpleak, NULL);
    return YES;
}
@end

int main(int argc, char *argv[]) {
    // devicectl rejects --console + environment-variables on this build
    // (CoreDevice 10002 EINVAL), so phase config is passed as KEY=VAL
    // command-line arguments and injected into the environment here.
    for (int i = 1; i < argc; i++) {
        const char *eq = strchr(argv[i], '=');
        if (eq && eq != argv[i]) {
            char key[128];
            size_t kl = (size_t)(eq - argv[i]);
            if (kl < sizeof(key)) {
                memcpy(key, argv[i], kl);
                key[kl] = 0;
                setenv(key, eq + 1, 1);
            }
        }
    }
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass(AppDelegate.class));
    }
}
