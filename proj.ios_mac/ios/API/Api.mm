//
//  Api.m
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//
//

#include "ApiBridge.h"
#include "GameData.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "IapObject.h"

#import "AppController.h"
#import "RootViewController.h"
#import "platform/ios/CCEAGLView-ios.h"
#import "RMStore.h"
#import "GameCenterManager.h"
#import "KeychainItemWrapper.h"
#import "NSString+Extended.h"

using namespace std;

// alert view
static AlertViewClickedButtonCallback alertViewClickedButtonCallback = nullptr;
long const IapVerificationFailedCode = RMStoreErrorCodeUnableToCompleteVerification;

#pragma mark - Private Class that handle objc callbacks
@interface Api : NSObject <
UIAlertViewDelegate,
RMStoreReceiptVerifier,
SKPaymentTransactionObserver,
GameCenterManagerDelegate
>

@end

@implementation Api
#pragma mark Alloc & Dealloc
- (instancetype)init
{
    if (self = [super init]) {
        
    }
    
    return self;
}

- (void)dealloc
{
    
}

#pragma mark UIAlertViewDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertViewClickedButtonCallback) {
        alertViewClickedButtonCallback(alertView.tag, buttonIndex);
    }
    
}

- (void)alertViewCancel:(UIAlertView *)alertView
{
    // do nothing, just incase the user click "Home" button
}

#pragma mark RMStoreReceiptVerifier
- (void)verifyTransaction:(SKPaymentTransaction*)transaction
                  success:(void (^)())successBlock
                  failure:(void (^)(NSError *error))failureBlock
{
#ifdef NDEBUG
    BOOL sandbox = NO;
#else
    BOOL sandbox = YES;
#endif
    
#if __IPHONE_OS_VERSION_MAX_ALLOWED < __IPHONE_7_0
    NSData *receipt = [transaction.transactionReceipt base64EncodedDataWithOptions:0];
#else
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:[NSBundle mainBundle].appStoreReceiptURL];
    NSError *error = nil;
    NSData *rawData = [NSURLConnection sendSynchronousRequest:urlRequest returningResponse:nil error:&error];
    NSData *receipt = [rawData base64EncodedDataWithOptions:0];
#endif
    string receiptData = [[NSString alloc] initWithData:receipt encoding:NSUTF8StringEncoding].stdString;
    NetworkApi::iap(sandbox, receiptData, [=](cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response) {
        if (NetworkApi::isSuccessfulResponse(response)) {
            rapidjson::Document jsonDict;
            NetworkApi::parseResponseData(response->getResponseData(), jsonDict);
            GameData::getInstance()->currentUser()->parseResources(jsonDict, "resources");
            
            if (successBlock) {
                successBlock();
            }
        }
        else if (failureBlock) {
            NSString *domain = [NSBundle mainBundle].bundleIdentifier;
            NSError *error = [NSError errorWithDomain:domain code:RMStoreErrorCodeUnableToCompleteVerification userInfo:nil];
            failureBlock(error);
        }
    });
}

#pragma mark SKPaymentTransactionObserver
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    // in case when the user hasn't finished transaction before
    bool isPurchasing = false;
    for (SKPaymentTransaction *transaction in transactions) {
        if (transaction.transactionState == SKPaymentTransactionStatePurchasing) {
            isPurchasing = true;
            break;
        }
    }
    
    if (isPurchasing) {
        GameData::getInstance()->beginTransaction();
    }
}

#pragma mark GameCenterManagerDelegate
- (void)gameCenterManager:(GameCenterManager *)manager authenticateUser:(UIViewController *)gameCenterLoginController
{
    
}
@end

#pragma mark - iOS API
static KeychainItemWrapper *_keychain = nil;
static Api *_api = nil;
void iOSApi::init()
{
    _keychain = [[KeychainItemWrapper alloc] initWithIdentifier:@"saved_user" accessGroup:nil];
    _api = [[Api alloc] init];
}

void iOSApi::resetKeychain()
{
    if (_keychain) {
        [_keychain resetKeychainItem];
    }
}

void iOSApi::loadAnonymousUser(rapidjson::Document& document)
{
    NSData *data = [_keychain objectForKey:(__bridge id)(kSecValueData)];
    if (data.length > 0) {
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data options:0 error:0];
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        // MUST alloc a new string, otherwise it will cause some bug
        static string kAuthString;
        int uid = [[dict objectForKey:[NSString stringWithUTF8String:kUID]] intValue];
        kAuthString.assign([[dict objectForKey:[NSString stringWithUTF8String:kAuth]] UTF8String]);
        bool isNew = [[dict objectForKey:[NSString stringWithUTF8String:kNew]] boolValue];
        
        document.AddMember(kUID, uid, allocator);
        document.AddMember(kAuth, rapidjson::Value(kAuthString.c_str(), allocator), allocator);
        document.AddMember(kNew, isNew, allocator);
    }
}

void iOSApi::saveAnonymousUser(const User* user)
{
    const string& token = user->getToken();
    if (token.size() > 0) {
        const unsigned int userId = user->getUid();
        // serialize
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        [dict setValue:[NSString stringWithFormat:@"%d", userId] forKey:[NSString stringWithUTF8String:kUID]];
        [dict setValue:[NSString stringWithUTF8String:token.c_str()] forKey:[NSString stringWithUTF8String:kAuth]];
        [dict setValue:[NSString stringWithFormat:@"%d", false] forKey:[NSString stringWithUTF8String:kNew]];
        NSData *data = [NSJSONSerialization dataWithJSONObject:dict options:0 error:0];
        
        [_keychain setObject:data forKey:(__bridge id)(kSecValueData)];
        [_keychain setObject:@(userId) forKey:(__bridge id)(kSecAttrAccount)];
        [_keychain setObject:(__bridge id)kSecAttrAccessibleWhenUnlocked forKey:(__bridge id)kSecAttrAccessible];
    }
    else {
        resetKeychain();
    }
}

void iOSApi::initIAPEnviroment()
{
    // only init iap after user is login
    RMStore *store = [RMStore defaultStore];
    [store addTransactionObserver];
    store.receiptVerifier = _api;
    [[SKPaymentQueue defaultQueue] addTransactionObserver:_api];
}

void iOSApi::getAppleIAPInfo(const IAPProductsRequestSuccessCallback& success, const IAPProductsRequestFailedCallback& failed)
{
    static IAPProductsRequestSuccessCallback successCallback = nullptr;
    static IAPProductsRequestFailedCallback failedCallback = nullptr;
    successCallback = success;
    failedCallback = failed;
    
    static NSSet *productIds = [NSSet setWithObjects:@"100_60", @"100_720", @"100_3860", @"100_8100", nil];
    NSMutableSet *products = [NSMutableSet set];
    for (NSString *productId in productIds) {
        [products addObject:productId.toAppleProductId];
    }
    [[RMStore defaultStore] requestProducts:products success:^(NSArray *products, NSArray *invalidProductIdentifiers) {
        // 1. sort
        NSArray *productsSortedByPrice = [products sortedArrayUsingComparator:^NSComparisonResult(SKProduct* obj1, SKProduct* obj2) {
            NSInteger price1 = [obj1.price integerValue];
            NSInteger price2 = [obj2.price integerValue];
            if (price1 < price2) {
                return NSOrderedAscending;
            } else if (price1 > price2) {
                return NSOrderedDescending;
            }
            return NSOrderedSame;
        }];
        
        vector<IapObject *> __block objects;
        for (SKProduct *product in productsSortedByPrice) {
            NSString *identifier = product.productIdentifier;
            NSString *key = [identifier pathExtension];
            IapObject *object = new IapObject([key UTF8String], (__bridge void *)product);
            objects.push_back(object);
        }
        
        // 2. callback
        if (successCallback) {
            successCallback(objects);
        }
        
        // 3. clear after reassign
        for (int i = 0; i < objects.size(); ++i) {
            CC_SAFE_DELETE(objects.at(i));
        }
        objects.clear();
    } failure:^(NSError *error) {
        if (failedCallback) {
            failedCallback();
        }
    }];
}

void iOSApi::buyGemApple(const string& account, int gemNumber, const IAPPaymentSuccessCallback& success, const IAPPaymentFailedCallback& failed)
{
    static IAPPaymentSuccessCallback successCallback = nullptr;
    static IAPPaymentFailedCallback failedCallback = nullptr;
    successCallback = success;
    failedCallback = failed;
    
    NSString *identifier = [NSString stringWithFormat:@"%d_%d", 100, gemNumber].toAppleProductId;
    [[RMStore defaultStore] addPayment:identifier user:[[NSString stringWithUTF8String:account.c_str()] hashedValueForAccount] success:^(SKPaymentTransaction *transaction) {
        if (successCallback) {
            successCallback();
        }        
    } failure:^(SKPaymentTransaction *transaction, NSError *error) {
        NSInteger code = error.code;
        if (code == RMStoreErrorCodeUnableToCompleteVerification)
        {
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        }
        if (failedCallback) {
            failedCallback([error.description stdString], code);
        }
    }];
}

#pragma mark - Game Center
void iOSApi::setupGCManager()
{
    [[GameCenterManager sharedManager] setupManager];
    [[GameCenterManager sharedManager] setDelegate:_api];
}

string iOSApi::getUUID()
{
    CFUUIDRef uuidRef = CFUUIDCreate(nil);
    CFStringRef uuidStringRef = CFUUIDCreateString(nil, uuidRef);
    CFRelease(uuidRef);
    
    NSString *uuidValue = (__bridge_transfer NSString *)uuidStringRef;
    uuidValue = [uuidValue lowercaseString];
    uuidValue = [uuidValue stringByReplacingOccurrencesOfString:@"-" withString:@""];
    return [uuidValue stdString];
}

string iOSApi::getVersionId()
{
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
    return [[info objectForKey:@"CFBundleShortVersionString"] stdString];
}

string iOSApi::getBuildId()
{
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
    return [[info objectForKey:@"CFBundleVersion"] stdString];
}

string iOSApi::getLanguage()
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];
    return [currentLanguage stdString];
}

void iOSApi::setMultipleTouchEnabled(bool enabled)
{
    auto ad = (AppController*)[UIApplication sharedApplication].delegate;
    CCEAGLView* view = (CCEAGLView*)ad.viewController.view;
    if (view) {
        [view setMultipleTouchEnabled:enabled];
    }
}

void iOSApi::showAlertView(const string& title, const string& message, bool showTwoButtons, int tag, const AlertViewClickedButtonCallback& callback)
{
    alertViewClickedButtonCallback = callback;
    
    NSString *_title = title.length() > 0 ? [NSString stringWithUTF8String:title.c_str()] : nil;
    NSString *_message = message.length() > 0 ? [NSString stringWithUTF8String:message.c_str()] : nil;
    
    NSString *_confirm = [NSString stringWithUTF8String:LocalHelper::getString("hint_confirm").c_str()];
    NSString *_cancel = [NSString stringWithUTF8String:LocalHelper::getString("hint_cancel").c_str()];
    
    UIAlertView * alertView = [[UIAlertView alloc] initWithTitle: _title
                                                          message: _message
                                                         delegate: _api
                                               cancelButtonTitle: showTwoButtons ? _cancel : _confirm
                                               otherButtonTitles: showTwoButtons ? _confirm : nil, nil];
    [alertView setTag:tag];
    [alertView show];
}

#pragma mark - IapObject
IapObject::IapObject(const string& key, void *value)
{
    SKProduct *product = (__bridge SKProduct *)value;
    
    NSString *k = [NSString stringWithUTF8String:key.c_str()];
    NSArray *array = [k componentsSeparatedByString:@"_"];
    _objectId = [array[0] intValue];
    _count = [array[1] intValue];
    
    // local price
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setLocale:product.priceLocale];
    _price = [[numberFormatter stringFromNumber:product.price] floatValue];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    _priceString = [[numberFormatter stringFromNumber:product.price] stdString];
    
    // title
    _title = [product.localizedTitle stdString];
    _description = [product.localizedDescription stdString];
}