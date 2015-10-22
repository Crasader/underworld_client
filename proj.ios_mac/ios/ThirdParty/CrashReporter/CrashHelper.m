//
//  CrashHelper.m
//  Underworld_Client
//
//  Created by Andy on 15/10/22.
//
//

#import "CrashHelper.h"
#import <sys/sysctl.h>
#import <CrashReporter/CrashReporter.h>
#import <MessageUI/MFMailComposeViewController.h>

static NSString * kDefaultEmail = @"pk08@sina.com";
static NSString * kReportFolder = @"mofish.plcrash";

#define __CHLOGWITHFUNCTION(s, ...) \
NSLog(@"%s : %@",__FUNCTION__,[NSString stringWithFormat:(s), ##__VA_ARGS__])

#if COCOS2D_DEBUG
#define CHLOG(...) __CHLOGWITHFUNCTION(__VA_ARGS__)
#else
#define CHLOG(...) do {} while (0)
#endif

static BOOL isDebuggerMode() {
#if !TARGET_OS_IPHONE
    return NO;
#endif
    
    struct kinfo_proc info;
    size_t info_size = sizeof(info);
    int name[4];
    
    name[0] = CTL_KERN;
    name[1] = KERN_PROC;
    name[2] = KERN_PROC_PID;
    name[3] = getpid();
    
    if (sysctl(name, 4, &info, &info_size, NULL, 0) == -1) {
        CHLOG(@"sysctl() failed: %s", strerror(errno));
        return NO;
    }
    
    if ((info.kp_proc.p_flag & P_TRACED) != 0)
        return YES;
    
    return NO;
}

static void postCrashCallback(siginfo_t *info, ucontext_t *uap, void *context) {
    // this is not async-safe, but this is a test implementation
    CHLOG(@"post crash callback: signo=%d, uap=%p, context=%p", info->si_signo, uap, context);
}

@interface CrashHelper()<MFMailComposeViewControllerDelegate>
{
    BOOL _isDebuggerMode;
    UIViewController *_parentViewController;
    BOOL _hasCrashReport;
    PLCrashReporter *_crashReporter;
    NSString *_crashReporterPath;
}

@end

@implementation CrashHelper

- (instancetype)init {
    if (self = [super init]) {
        _isDebuggerMode = isDebuggerMode();
        if (_isDebuggerMode) {
            CHLOG(@"The demo crash app should be run without a debugger present. Exiting ...");
        } else {
            /* Configure our reporter */
            PLCrashReporterConfig *config = [[PLCrashReporterConfig alloc] initWithSignalHandlerType: PLCrashReporterSignalHandlerTypeMach
                                                                               symbolicationStrategy: PLCrashReporterSymbolicationStrategyAll];
            _crashReporter = [[PLCrashReporter alloc] initWithConfiguration:config];
            
            _hasCrashReport = [_crashReporter hasPendingCrashReport];
            
            /* Save any existing crash report. */
            [self saveCrashReport:_crashReporter];
        }
    }
    
    return self;
}

#pragma mark - public

+ (instancetype)sharedHelper {
    static CrashHelper *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[[self class] alloc] init];
    });
    return sharedInstance;
}

- (void)activateCrashReporter {
    /* Set up post-crash callbacks */
    PLCrashReporterCallbacks cb = {
        .version = 0,
        .context = (void *) 0xABABABAB,
        .handleSignal = postCrashCallback
    };
    
    [_crashReporter setCrashCallbacks:&cb];
    
    /* Enable the crash reporter */
    NSError *error = nil;
    if (![_crashReporter enableCrashReporterAndReturnError: &error]) {
        CHLOG(@"Could not enable crash reporter: %@", error);
    }
}

- (void)sendCrashReportWithEmail:(UIViewController *)parentViewController {
    if (true) {
        _parentViewController = parentViewController;
        
        if ([MFMailComposeViewController canSendMail] && [[NSFileManager defaultManager] fileExistsAtPath:_crashReporterPath]) {
            MFMailComposeViewController *controller = [[MFMailComposeViewController alloc] init];
            controller.mailComposeDelegate = self;
            NSArray *receipients = [[NSArray alloc]initWithObjects:kDefaultEmail, nil];
            [controller setToRecipients:receipients];
            [controller setSubject:@"Crash Report"];
            NSData *crashData = [NSData dataWithContentsOfFile:_crashReporterPath];
            [controller addAttachmentData:crashData mimeType:@"text/plain" fileName:@"Crash Report"];
            [controller setMessageBody:@"" isHTML:YES];
            [_parentViewController presentViewController:controller animated:YES completion:nil];
        } else {
            CHLOG(@"The device cannot email crash report");
        }
    }
}

#pragma mark - private

- (void)purgePreviewCrashReport:(PLCrashReporter *)reporter {
    NSError *error = nil;
    [reporter purgePendingCrashReportAndReturnError:&error];
}

- (void)saveCrashReport:(PLCrashReporter *)reporter {
    if (_hasCrashReport) {
#if TARGET_OS_IPHONE
        NSFileManager *fm = [NSFileManager defaultManager];
        NSError *error = nil;
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        if (![fm createDirectoryAtPath: documentsDirectory withIntermediateDirectories: YES attributes:nil error: &error]) {
            CHLOG(@"Could not create documents directory: %@", error);
            return;
        }
        
        NSData *data = [reporter loadPendingCrashReportDataAndReturnError: &error];
        if (data == nil) {
            CHLOG(@"Failed to load crash report data: %@", error);
            return;
        }
        
        _crashReporterPath = [documentsDirectory stringByAppendingPathComponent: kReportFolder];
        if (![data writeToFile: _crashReporterPath atomically: YES]) {
            CHLOG(@"Failed to write crash report");
            return;
        }
        
        CHLOG(@"Saved crash report to: %@", _crashReporterPath);
#endif
    }
}

#pragma mark MFMailComposeViewControllerDelegate
- (void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(nullable NSError *)error {
    if (result == MFMailComposeResultSent) {
        CHLOG(@"Crash report sent");
        [self purgePreviewCrashReport:_crashReporter];
    }
    [_parentViewController dismissViewControllerAnimated:YES completion:nil];
}
@end
