//
//  User.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "User.h"
#include "JSonUtils.h"
#include "UserDefaultsDataManager.h"
#include "NetworkApi.h"

using namespace std;

static inline const char* getFirstLoginKey() { return "first_entry"; }
static inline string getFinishedTutorialKey(int idx)
{ return cocos2d::StringUtils::format("tutorial_%d", idx); }

User::User(const rapidjson::Value& jsonDict)
:_isNewUser(false)
,_isGuest(false)
,_uid(0)
,_isFirstLogin(true)
,_finishedTutorialsMaxIdx(-1)
{
    // parse with the data from network
    JSonUtils::parse(_uid, jsonDict, kUID);
    JSonUtils::parse(_token, jsonDict, kAuth);
    JSonUtils::parse(_isNewUser, jsonDict, kNew);
    
    init();
}

User::~User()
{
    
}

#pragma mark - public
int User::getUid() const
{
    return _uid;
}

bool User::isNewUser() const
{
    return _isNewUser;
}

bool User::isGuest() const
{
    return _isGuest;
}

const string& User::getToken() const
{
    return _token;
}

const string& User::getName() const
{
    return _name;
}

#pragma mark - tags
bool User::isFirstLogin() const
{
    return _isFirstLogin;
}

void User::saveFirstLoginTag()
{
    static const bool value(false);
    _isFirstLogin = value;
    UserDefaultsDataManager::setBoolForKey(getFirstLoginKey(), value);
}

bool User::hasFinishedTutorial(int idx) const
{
    return (idx <= _finishedTutorialsMaxIdx);
}

void User::saveFinishedTutorialTag(int idx)
{
    if (idx > _finishedTutorialsMaxIdx) {
        _finishedTutorialsMaxIdx = idx;
        UserDefaultsDataManager::setBoolForKey(getFinishedTutorialKey(idx).c_str(), true);
    } else {
        assert(false);
    }
}

#pragma mark - user
void User::parseGemInfo(const rapidjson::Value& root)
{
    
}

void User::parseResources(const rapidjson::Value& root, const char* key, bool silent)
{
    
}

void User::loadUserInfo(const string& deviceToken, const httpRequestCallback& success, const httpErrorCallback& onError)
{
    NetworkApi::loadUserInfo(deviceToken, [=](long code, const rapidjson::Value& jsonDict) {
        if (HttpSuccessCode == code) {
            parseUserInfo(jsonDict);
            if (success) {
                success();
            }
        } else if (onError) {
            onError(code);
        }
    });
}

void User::init()
{
    // load data which is saved in local
    _isFirstLogin = UserDefaultsDataManager::getBoolForKey(getFirstLoginKey(), true);
    
    for (int i = 0; ; ++i) {
        if (false == UserDefaultsDataManager::getBoolForKey(getFinishedTutorialKey(i).c_str(), false)) {
            _finishedTutorialsMaxIdx = i - 1;
            break;
        }
    }
}

void User::parseUserInfo(const rapidjson::Value& jsonDict)
{
    
}