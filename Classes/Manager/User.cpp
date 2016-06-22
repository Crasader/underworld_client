//
//  User.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "User.h"
#include "cocostudio/CocoStudio.h"
#include "UserDefaultsDataManager.h"
#include "NetworkApi.h"
#include "FormationData.h"

using namespace std;
using namespace cocostudio;

static inline const char* getFirstLoginKey() { return "first_entry"; }
static inline string getFinishedTutorialKey(int idx)
{ return cocos2d::StringUtils::format("tutorial_%d", idx); }

static inline string getFormationKey(int idx)
{ return cocos2d::StringUtils::format("formation_%d", idx); }

static inline const char* getDefaultFormationKey() { return "default_formation"; }

User::User(const rapidjson::Document& jsonDict)
:_isFirstLogin(true)
,_finishedTutorialsMaxIdx(-1)
,_defaultFormationId(0)
{
    // parse with the data from network
    _uid = DICTOOL->getIntValue_json(jsonDict, kUID);
    _token = DICTOOL->getStringValue_json(jsonDict, kAuth, "");
    _isNewUser = DICTOOL->getBooleanValue_json(jsonDict, kNew);
    
    init();
}

User::~User()
{
    for (int i = 0; i < _formations.size(); ++i) {
        CC_SAFE_DELETE(_formations.at(i));
    }
}

#pragma mark - public
unsigned int User::getUserId() const
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

#pragma mark - formation
int User::getDefaultFormationId() const
{
    return _defaultFormationId;
}

void User::setDefaultFormationId(int value)
{
    _defaultFormationId = value;
    UserDefaultsDataManager::setIntegerForKey(getDefaultFormationKey(), value);
}

FormationData* User::getFormationData(int idx) const
{
    if (_formations.find(idx) != end(_formations)) {
        return _formations.at(idx);
    }
    
    return nullptr;
}

void User::saveFormationData(int idx)
{
    if (_formations.find(idx) != end(_formations)) {
        string output;
        auto data = _formations.at(idx);
        data->serialize(output);
        UserDefaultsDataManager::setStringForKey(getFormationKey(idx).c_str(), output);
    }
}

FormationData* User::getDefaultFormationData() const
{
    return getFormationData(_defaultFormationId);
}

void User::saveDefaultFormationData()
{
    saveFormationData(_defaultFormationId);
}

void User::parseGemInfo(const rapidjson::Value& root)
{
    
}

void User::parseResources(const rapidjson::Value& root, const char* key, bool silent)
{
    
}

void User::loadUserInfo(const string& deviceToken, const httpRequestCallback& success, const httpErrorCallback& onError)
{
    NetworkApi::loadUserInfo(deviceToken, [=](cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response) {
        if (NetworkApi::isSuccessfulResponse(response))
        {
            rapidjson::Document jsonDict;
            NetworkApi::parseResponseData(response->getResponseData(), jsonDict);
            parseUserInfo(jsonDict);
            
            if (success)
            {
                success();
            }
        }
        else
        {
            const long code = response->getResponseCode();
            if (onError) {
                onError(code);
            }
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
    
    for (int i = 0; i < FORMATION_MAX_COUNT; ++i) {
        const auto& string = UserDefaultsDataManager::getStringForKey(getFormationKey(i).c_str(), "");
        auto data = new FormationData(string);
        _formations.insert(make_pair(i, data));
    }
}

void User::parseUserInfo(const rapidjson::Document& jsonDict)
{
    
}