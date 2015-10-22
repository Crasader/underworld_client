//
//  CrashHelper.h
//  Underworld_Client
//
//  Created by Andy on 15/10/22.
//
//

#import <Foundation/Foundation.h>

@interface CrashHelper : NSObject

+ (instancetype)sharedHelper;
- (void)activateCrashReporter;
- (void)sendCrashReportWithEmail:(UIViewController *)parentViewController;

@end
