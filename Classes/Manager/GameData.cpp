//
//  GameData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GameData.h"
#include "cocostudio/CocoStudio.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#include "TargetConditionals.h"
#endif
#include "Utils.h"
#include "LocalHelper.h"
#include "IapObject.h"
#include "ProgressLayer.h"
#include "MessageBoxLayer.h"
#include "MainLayer.h"
#include "Constants.h"
#include "DataManager.h"
#include "NetworkApi.h"

using namespace std;
using namespace cocostudio;

static const string heartbeatScheduleKey("heartbeatScheduleKey");
static const float heartbeatInterval = 2 * 60.0f;

static bool sort_by_price(const IapObject *a, const IapObject *b)
{
    return a->price() < b->price();
}

GameData::GameData()
:_user(nullptr)
,_uuid(0)
,_isTransacting(nullptr)
{
    generateUUID();
    fetchIAPInfo(nullptr, nullptr, true);
    
    // TODO: remove test code
    {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        // MUST alloc a new string, otherwise it will cause some bug
        static string kAuthString;
        kAuthString.assign("asdf");
        document.AddMember(kUID, 1, allocator);
        document.AddMember(kAuth, rapidjson::Value(kAuthString.c_str(), allocator), allocator);
        document.AddMember(kNew, false, allocator);
        
        _user = new User(document);
    }
}

GameData::~GameData()
{
    CC_SAFE_DELETE(_user);
}

static GameData* s_pSharedInstance = nullptr;

GameData * GameData::getInstance()
{
    if (s_pSharedInstance == nullptr)
    {
        s_pSharedInstance = new (nothrow) GameData();
    }
    return s_pSharedInstance;
}

void GameData::purge()
{
    if (s_pSharedInstance)
    {
        delete s_pSharedInstance;
        s_pSharedInstance = nullptr;
    }
}

#pragma mark - public
void GameData::init() const
{
    parseFiles();
}

void GameData::parseFiles() const
{
    Constants::init();
    DataManager::getInstance()->init();
}

User* GameData::currentUser() const
{
    return _user;
}

void GameData::setDeviceToken(const string& token)
{
    _deviceToken.assign(token);
}

unsigned int GameData::getUUID() const
{
    return _uuid;
}

const string& GameData::getVersionId() const
{
    return _versionId;
}

#pragma mark - login & logout
void GameData::autoLogin(const httpRequestCallback& success, const httpErrorCallback& failed)
{
    // 1. load user's info from local
    rapidjson::Document document;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    iOSApi::loadAnonymousUser(document);
#else
    loadAnonymousUser(document);
#endif
    const string token = cocostudio::DICTOOL->getStringValue_json(document, kAuth, "");
    // 2. if the token is saved in local
    if (token.length() == USER_TOKEN_LENGTH)
    {
        _user = new (std::nothrow) User(document);
        _user->loadUserInfo(_deviceToken, success, failed);
    }
    else
    {
        if (failed) {
            failed(InvalidTokenErrorCode);
        }
    }
}

void GameData::login(const httpRequestCallback& success, const httpErrorCallback& failed)
{
    // 1. load user's info from local
    rapidjson::Document document;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    iOSApi::loadAnonymousUser(document);
#else
    loadAnonymousUser(document);
#endif
    const string token = cocostudio::DICTOOL->getStringValue_json(document, kAuth, "");
    // 2. if the token is saved in local
    if (token.length() == USER_TOKEN_LENGTH)
    {
        _user = new (std::nothrow) User(document);
        _user->loadUserInfo(_deviceToken, success, [=](int code) {
            if (InvalidTokenErrorCode == code) {
                CC_SAFE_DELETE(_user);
                _user = nullptr;
                requestLogin(success, failed);
            } else {
                MessageBoxLayer::getInstance()->show(LocalHelper::getString("networkResponse_loadUserInfoFailed"));
            }
        });
    }
    // 3. if not
    else
    {
//        CCASSERT(token.empty(), "The token is interrupted.");
        requestLogin(success, failed);
    }
}

bool GameData::isLoggedIn() const
{
    return _user != nullptr;
}

#pragma mark - iap
bool GameData::isTransacting() const
{
    return _isTransacting;
}

void GameData::beginTransaction()
{
    _isTransacting = true;
}

void GameData::finishTransaction()
{
    _isTransacting = false;
}

void GameData::fetchIAPInfo(const httpRequestCallback& success, const httpRequestCallback& failed, bool showLoadingLayer)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (showLoadingLayer) {
        ProgressLayer::showSingleton();
    }
    iOSApi::getAppleIAPInfo([=](const vector<IapObject *>& objects) {
        if (showLoadingLayer) {
            ProgressLayer::hideSingleton();
        }
        const ssize_t size = objects.size();
        if (size > 0)
        {
            _iapObjects.clear();
            for (int i = 0; i < size; ++i)
            {
                const IapObject& instance = *(objects.at(i));
                _iapObjects.push_back(new IapObject(instance));
            }
            
            sort(_iapObjects.begin(), _iapObjects.end(), sort_by_price);
            for (int i = 0; i < _iapObjects.size(); ++i)
            {
                _iapObjects.at(i)->setIndex(i);
            }
            
            if (success) {
                success();
            }
        }
    }, [=]() {
        if (showLoadingLayer) {
            ProgressLayer::hideSingleton();
        }
        if (failed) {
            failed();
        }
    });
#endif
}

const vector<IapObject *>& GameData::getIapObjects() const
{
    return _iapObjects;
}

#pragma mark - load user info
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
void GameData::loadAnonymousUser(rapidjson::Document& document)
{
//    UserDefault *ud = UserDefault::getInstance();
//    document.SetObject();
//    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
//    // MUST alloc a new string, otherwise it will cause some bug
//    static string kAuthString;
//    kAuthString.assign(ud->getStringForKey(kAuth));
//    
//    document.AddMember(kUID, ud->getIntegerForKey(kUID), allocator);
//    document.AddMember(kAuth, rapidjson::Value(kAuthString.c_str(), allocator), allocator);
//    document.AddMember(kNew, ud->getBoolForKey(kNew), allocator);
}

void GameData::saveAnonymousUser(const User* user)
{
//    UserDefault *ud = UserDefault::getInstance();
//    ud->setIntegerForKey(kUID, user->uid());
//    ud->setStringForKey(kAuth, user->token());
//    ud->setBoolForKey(kNew, false);
//    ud->flush();
}

#endif  // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

void GameData::generateUUID()
{
    static const char* key("uuid_key");
    UserDefault *ud = UserDefault::getInstance();
    _uuid = ud->getIntegerForKey(key, 0);
    if (0 == _uuid) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        const string& uuid = iOSApi::getUUID();
        _uuid = Utils::bkdrHash(uuid.c_str());
#else
        random_device rd;
        mt19937 mt(rd());
        _uuid = mt();
        MessageBox(StringUtils::format("UUID: %d", _uuid).c_str(), nullptr);
#endif
        ud->setIntegerForKey(key, _uuid);
        ud->flush();
    }
}

void GameData::requestLogin(const httpRequestCallback& success, const httpErrorCallback& failed)
{
    if (false == isLoggedIn())
    {
        NetworkApi::login([=](cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response) {
            handleLoginResponse(response, success, failed);
        });
    }
}

void GameData::handleLoginResponse(cocos2d::network::HttpResponse* response, const httpRequestCallback& success, const httpErrorCallback& failed)
{
    if (NetworkApi::isSuccessfulResponse(response))
    {
        rapidjson::Document jsonDict;
        NetworkApi::parseResponseData(response->getResponseData(), jsonDict);
        _user = new (std::nothrow) User(jsonDict);
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        iOSApi::saveAnonymousUser(_user);
#else
        saveAnonymousUser(_user);
#endif
        _user->loadUserInfo(_deviceToken, success, failed);
    }
    else
    {
        if (failed) {
            failed(InvalidTokenErrorCode);
        }
    }
}

void GameData::reloadGame()
{
    // 1. reset memory
    Director::getInstance()->getScheduler()->unschedule(heartbeatScheduleKey, this);
    CC_SAFE_DELETE(_user);
    
    // 2. reload
    
    // 3. update UI
    auto director = Director::getInstance();
    auto scene = MainLayer::createScene();
    director->purgeCachedData();
    director->popToRootScene();
    director->replaceScene(scene);
}

#pragma mark - heartbeat
void GameData::startHeartBeatSynchronously()
{
    heartBeatRequest(true);
}

void GameData::scheduleHeartBeat()
{
    Scheduler *scheduler = Director::getInstance()->getScheduler();
    scheduler->unschedule(heartbeatScheduleKey, this);
    scheduler->schedule(bind(&GameData::heartBeat, this, placeholders::_1), this, heartbeatInterval, CC_REPEAT_FOREVER, 0.0f, false, heartbeatScheduleKey);
}

void GameData::heartBeat(float dt)
{
    heartBeatRequest(false);
}

void GameData::heartBeatRequest(bool showLoadingView)
{
    NetworkApi::heartBeat([this](cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response) {
        if (NetworkApi::isSuccessfulResponse(response))
        {
            rapidjson::Document jsonDict;
            NetworkApi::parseResponseData(response->getResponseData(), jsonDict);
            bool online = cocostudio::DICTOOL->getBooleanValue_json(jsonDict, "online");
            if (false == online)
            {
                onUserIsOffline();
            }
        }
    }, showLoadingView);
}

void GameData::stopHeartBeat()
{
    Scheduler *scheduler = Director::getInstance()->getScheduler();
    scheduler->unschedule(heartbeatScheduleKey, this);
}

#pragma mark - callbacks
void GameData::onLogout()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::resetKeychain();
#endif
    reloadGame();
}

void GameData::onUpdateVersionID(const string& version)
{
    _versionId.assign(version);
}

void GameData::onNetworkResponseFailed(long code, const string& jsonPath) const
{
#if COCOS2D_DEBUG
#if TARGET_IPHONE_SIMULATOR
    MessageBox(StringUtils::format(LocalHelper::getString("networkResponse_errorCode").c_str(), code, jsonPath.c_str()).c_str(), LocalHelper::getString("networkResponse_errorTitle").c_str());
#endif
#endif
}

void GameData::onVersionError()
{
    MessageBoxLayer::getInstance()->show(LocalHelper::getString("networkResponse_oldVersion"), MessageBoxType::OnlyYes, [this](Ref *) {
        reloadGame();
    });
}

void GameData::onUserIsOnline()
{
    scheduleHeartBeat();
}

void GameData::onUserIsOffline()
{
    MessageBoxLayer::getInstance()->show(LocalHelper::getString("networkResponse_isOffline"), MessageBoxType::OnlyYes, [this](Ref *) {
        reloadGame();
    });
}