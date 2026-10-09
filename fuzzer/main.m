#import <UIKit/UIKit.h>
#import "fuzz.h"
#import <os/lock.h>
#import <stdatomic.h>
#import <stdarg.h>
#import <time.h>
#import <sys/time.h>

// ---------------------------------------------------------------------------
// Live on-screen console (просьба оператора: «ультра-подробный лог всего,
// что происходит» — на экране устройства, не только в Documents/fuzz.log).
//
// Каждая строка LOG() из любой фазы/потока попадает сюда с временными
// метками до миллисекунд. Потокобезопасно: очередь под os_unfair_lock,
// сброс в UITextView скоалесцирован на main (~10 Гц), автоскролл к низу
// (пока оператор сам не поднял просмотр вверх). Кишечник ограничен
// 6000 строк в очереди / ~300 КБ текста в UITextView.
// ---------------------------------------------------------------------------

static NSMutableArray<NSString *> *gLogLines;   // под gLogLock
static NSUInteger gLogFlushed;                  // индекс следующей несданной
static os_unfair_lock gLogLock = OS_UNFAIR_LOCK_INIT;
static atomic_bool gFlushScheduled = false;
static UITextView *gLogView;                    // main thread only
static UILabel *gLogHeader;                     // main thread only
static NSDictionary<NSAttributedStringKey, id> *gLogAttrs;

static void fzlog_flush(void);

void fzlog_emit(const char *fmt, ...) {
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n < 0) return;
    fprintf(stderr, "%s\n", buf);   // прежний канал: devicectl / fuzz.log

    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm tmv;
    localtime_r(&tv.tv_sec, &tmv);
    char ts[24];
    int tslen = snprintf(ts, sizeof(ts), "%02d:%02d:%02d.%03d ",
                         tmv.tm_hour, tmv.tm_min, tmv.tm_sec,
                         (int)(tv.tv_usec / 1000));
    NSString *line = [[NSString alloc] initWithBytesNoCopy:ts
                                                    length:(NSUInteger)tslen
                                                  encoding:NSUTF8StringEncoding
                                              freeWhenDone:NO];
    line = [line stringByAppendingFormat:@"%s", buf];

    if (!gLogLines) {
        static dispatch_once_t once;
        dispatch_once(&once, ^{
            gLogLines = [[NSMutableArray alloc] initWithCapacity:4096];
        });
    }
    os_unfair_lock_lock(&gLogLock);
    if (gLogLines.count >= 6000) {
        NSUInteger drop = 1000;
        [gLogLines removeObjectsInRange:NSMakeRange(0, drop)];
        gLogFlushed = (gLogFlushed > drop) ? gLogFlushed - drop : 0;
    }
    [gLogLines addObject:line];
    os_unfair_lock_unlock(&gLogLock);

    if (atomic_exchange(&gFlushScheduled, true)) return;
    dispatch_async(dispatch_get_main_queue(), ^{
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,
                                     (int64_t)(0.08 * NSEC_PER_SEC)),
                       dispatch_get_main_queue(), ^{
            atomic_store(&gFlushScheduled, false);
            fzlog_flush();
        });
    });
}

static void fzlog_flush(void) {
    if (!gLogView) return;
    NSMutableArray<NSString *> *fresh = [NSMutableArray array];
    NSUInteger total = 0;
    os_unfair_lock_lock(&gLogLock);
    if (gLogFlushed < gLogLines.count) {
        NSUInteger len = gLogLines.count - gLogFlushed;
        [fresh addObjectsFromArray:
            [gLogLines subarrayWithRange:NSMakeRange(gLogFlushed, len)]];
        gLogFlushed = gLogLines.count;
    }
    total = gLogLines.count;
    os_unfair_lock_unlock(&gLogLock);
    if (fresh.count) {
        NSString *chunk = [[fresh componentsJoinedByString:@"\n"]
                           stringByAppendingString:@"\n"];
        NSAttributedString *as = [[NSAttributedString alloc]
            initWithString:chunk attributes:gLogAttrs];
        // автоскролл — только если оператор не отмотал вверх сам
        BOOL atBottom = (gLogView.contentSize.height
                         - gLogView.contentOffset.y
                         - gLogView.bounds.size.height) < 80.0;
        [gLogView.textStorage appendAttributedString:as];
        if (gLogView.textStorage.length > 300000) {
            NSUInteger over = gLogView.textStorage.length - 260000;
            NSRange nl = [gLogView.textStorage.string
                rangeOfString:@"\n"
                       options:0
                         range:NSMakeRange(0, over)];
            if (nl.location != NSNotFound)
                [gLogView.textStorage deleteCharactersInRange:
                    NSMakeRange(0, nl.location + 1)];
        }
        if (atBottom)
            [gLogView scrollRangeToVisible:
                NSMakeRange(gLogView.textStorage.length, 0)];
    }
    gLogHeader.text = [NSString stringWithFormat:
        @" FUZZ LIVE LOG  lines=%lu  (fuzz.log tee: FUZZ_LOGFILE)",
        (unsigned long)total];
}

@interface FzLogVC : UIViewController
@end
@implementation FzLogVC
- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = UIColor.blackColor;
    gLogHeader = [[UILabel alloc] init];
    gLogHeader.font = [UIFont monospacedSystemFontOfSize:11
                                                  weight:UIFontWeightSemibold];
    gLogHeader.textColor = UIColor.systemGreenColor;
    gLogHeader.backgroundColor = [UIColor colorWithWhite:0.10 alpha:1.0];
    gLogHeader.text = @" FUZZ LIVE LOG";
    gLogView = [[UITextView alloc] init];
    gLogView.editable = NO;
    gLogView.selectable = YES;
    gLogView.backgroundColor = UIColor.blackColor;
    gLogView.textColor = UIColor.whiteColor;
    gLogView.font = [UIFont fontWithName:@"Menlo-Regular" size:10]
        ?: [UIFont monospacedSystemFontOfSize:10 weight:UIFontWeightRegular];
    gLogView.alwaysBounceVertical = YES;
    gLogView.textContainerInset = UIEdgeInsetsMake(4, 6, 4, 6);
    gLogAttrs = @{ NSFontAttributeName: gLogView.font,
                   NSForegroundColorAttributeName: UIColor.whiteColor };
    [self.view addSubview:gLogHeader];
    [self.view addSubview:gLogView];
}
- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    CGFloat top = self.view.safeAreaInsets.top;
    CGFloat bot = self.view.safeAreaInsets.bottom;
    CGFloat w = self.view.bounds.size.width;
    CGFloat h = self.view.bounds.size.height;
    gLogHeader.frame = CGRectMake(0, top, w, 22);
    gLogView.frame = CGRectMake(0, top + 22, w, h - top - 22 - bot);
}
@end

@interface FzSceneDelegate : UIResponder <UIWindowSceneDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation FzSceneDelegate
// Окно создаётся ЗДЕСЬ и привязывается к UIWindowScene. Прежний путь
// (окно в AppDelegate без windowScene) на iOS 27 не показывался вовсе —
// отсюда «пропавшая» надпись «fuzzing — see syslog» и пустой экран.
- (void)scene:(UIScene *)scene
    willConnectToSession:(UISceneSession *)session
                 options:(UISceneConnectionOptions *)options {
    if (![scene isKindOfClass:UIWindowScene.class]) return;
    UIWindowScene *ws = (UIWindowScene *)scene;
    self.window = [[UIWindow alloc] initWithWindowScene:ws];
    self.window.rootViewController = [FzLogVC new];
    [self.window makeKeyAndVisible];
}
@end

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@end

@implementation AppDelegate
// iOS 27 hard-crashes apps without scene-lifecycle adoption at scene
// creation (SIGTRAP, __UIApplicationEvaluateRuntimeIssueForNoSceneLifecycleAdoption).
// Providing a scene configuration WITH a delegate class satisfies the check
// and makes the UI actually appear (see FzSceneDelegate above).
- (UISceneConfiguration *)application:(UIApplication *)app
    configurationForConnectingSceneSession:(UISceneSession *)session
                                   options:(UISceneConnectionOptions *)opts {
    UISceneConfiguration *cfg =
        [[UISceneConfiguration alloc] initWithName:@"Default" sessionRole:session.role];
    cfg.delegateClass = FzSceneDelegate.class;
    return cfg;
}
- (BOOL)application:(UIApplication *)app didFinishLaunchingWithOptions:(NSDictionary *)opts {
    // экран не гаснет, пока оператор смотрит лог фаз
    app.idleTimerDisabled = YES;

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
    // ультра-детализация: вся конфигурация фаз из окружения на видном месте
    extern char **environ;
    for (char **e = environ; *e; e++)
        if (strncmp(*e, "FUZZ_", 5) == 0)
            LOG("[main] env %s", *e);
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
