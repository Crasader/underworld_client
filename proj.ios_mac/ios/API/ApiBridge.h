//
//  ApiBridge.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//
//

#ifndef ZombiePop_ApiBridge_h
#define ZombiePop_ApiBridge_h

#include "NetworkApi.h"
#include <string>
#include <vector>
#include <map>

class User;
class IapObject;

extern long const IapVerificationFailedCode;

typedef std::function<void(long alertViewTag, long buttonIndex)> AlertViewClickedButtonCallback;
typedef std::function<void(const std::vector<IapObject *>& objects)> IAPProductsRequestSuccessCallback;
typedef std::function<void()> IAPProductsRequestFailedCallback;
typedef std::function<void()> IAPPaymentSuccessCallback;
typedef std::function<void(const std::string& description, long code)> IAPPaymentFailedCallback;

namespace iOSApi {
    
    void init();
    
    // anonymous
    void resetKeychain();
    void loadAnonymousUser(rapidjson::Document& document);
    void saveAnonymousUser(const User* user);
    
    // iap
    void initIAPEnviroment();
    void getAppleIAPInfo(const IAPProductsRequestSuccessCallback& success, const IAPProductsRequestFailedCallback& failed);
    void buyGemApple(const std::string& account, int gemNumber, const IAPPaymentSuccessCallback& success, const IAPPaymentFailedCallback& failed);
    
    // utils
    std::string getUUID();
    std::string getVersionId();
    std::string getBuildId();
    void setMultipleTouchEnabled(bool enabled);
    void showAlertView(const std::string& title, const std::string& message, bool showTwoButtons, int tag, const AlertViewClickedButtonCallback& callback);
}

#endif  // #ifndef ZombiePop_ApiBridge_h