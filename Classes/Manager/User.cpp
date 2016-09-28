//
//  User.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "User.h"
#include "JSonUtils.h"
#include "UserDefaultHelper.h"
#include "ResourceManager.h"
#include "NetworkApi.h"
#include "UserSimpleData.h"

using namespace std;

static inline const char* getFirstLoginKey() { return "first_entry"; }
static inline string getFinishedTutorialKey(int idx)
{ return cocos2d::StringUtils::format("tutorial_%d", idx); }

static const char* kGUEST("guest");
static const char* kNEW("new_user");

User::User(const rapidjson::Value& jsonDict)
:_isNewUser(false)
,_isGuest(false)
,_isInBattle(false)
,_isFirstLogin(true)
,_finishedTutorialsMaxIdx(-1)
,_userData(nullptr)
{
    // parse with the data from network
    JSonUtils::parse(_isGuest, jsonDict, kGUEST);
    JSonUtils::parse(_token, jsonDict, kAuth);
    JSonUtils::parse(_isNewUser, jsonDict, kNEW);
    
    init();
}

User::~User()
{
    CC_SAFE_DELETE(_userData);
}

#pragma mark - public
bool User::isNewUser() const
{
    return _isNewUser;
}

bool User::isGuest() const
{
    return _isGuest;
}

bool User::isInBattle() const
{
    return _isInBattle;
}

int User::getUid() const
{
    if (_userData) {
        return _userData->getUid();
    }
    
    return 0;
}

const string& User::getToken() const
{
    return _token;
}

UserSimpleData* User::getUserData() const
{
    return _userData;
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
    UserDefaultHelper::setBoolForKey(getFirstLoginKey(), value);
}

bool User::hasFinishedTutorial(int idx) const
{
    return (idx <= _finishedTutorialsMaxIdx);
}

void User::saveFinishedTutorialTag(int idx)
{
    if (idx > _finishedTutorialsMaxIdx) {
        _finishedTutorialsMaxIdx = idx;
        UserDefaultHelper::setBoolForKey(getFinishedTutorialKey(idx).c_str(), true);
    } else {
        assert(false);
    }
}

#pragma mark - user
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
    _isFirstLogin = UserDefaultHelper::getBoolForKey(getFirstLoginKey(), true);
    
    for (int i = 0; ; ++i) {
        if (false == UserDefaultHelper::getBoolForKey(getFinishedTutorialKey(i).c_str(), false)) {
            _finishedTutorialsMaxIdx = i - 1;
            break;
        }
    }
}

void User::parseUserInfo(const rapidjson::Value& jsonDict)
{
    do {
        CC_BREAK_IF(JSonUtils::parse<int>(jsonDict, "code") != 0);
        JSonUtils::parse(_isInBattle, jsonDict, "inbattle");
        ResourceManager::getInstance()->updateResources(jsonDict);
        
        static const char* key("user");
        CC_BREAK_IF(!JSonUtils::isExist(jsonDict, key));
        const auto& value = DICTOOL->getSubDictionary_json(jsonDict, key);
        JSonUtils::parse(_isGuest, value, kGUEST);
        if (_userData) {
            _userData->update(value);
        } else {
            _userData = new (std::nothrow) UserSimpleData(value);
        }
    } while (false);
}
