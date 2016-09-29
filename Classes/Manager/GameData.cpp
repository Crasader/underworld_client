//
//  GameData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GameData.h"
#include "JSonUtils.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#include "TargetConditionals.h"
#endif
#include "Utils.h"
#include "LocalHelper.h"
#include "MessageBoxLayer.h"
#include "MainScene.h"
#include "Constants.h"
#include "DataManager.h"
#include "NetworkApi.h"
#include "NetworkController.h"

using namespace std;

static const string heartbeatScheduleKey("heartbeatScheduleKey");
static const float heartbeatInterval = 2 * 60.0f;

GameData::GameData()
:_user(nullptr)
,_uuid(0)
{
    generateUUID();
    
#if !ENABLE_LOGIN
    do {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        // MUST alloc a new string, otherwise it will cause some bug
        static string kAuthString;
        kAuthString.assign("asdf");
        document.AddMember(kUID, 1, allocator);
        document.AddMember(kAuth, rapidjson::Value(kAuthString.c_str(), allocator), allocator);
        
        _user = new User(document);
    } while (false);
#endif
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
    // TODO: set some global parameters
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

int GameData::getUUID() const
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
    loadAnonymousUser(document);
    const string token = JSonUtils::parse<string>(document, kAuth);
    // 2. if the token is saved in local
    if (token.length() == USER_TOKEN_LENGTH) {
        _user = new (nothrow) User(document);
        _user->loadUserInfo(_deviceToken, success, failed);
    } else if (failed) {
        failed(InvalidTokenErrorCode);
    }
}

void GameData::login(const httpRequestCallback& success, const httpErrorCallback& failed)
{
    // 1. load user's info from local
    rapidjson::Document document;
    loadAnonymousUser(document);
    const string token = JSonUtils::parse<string>(document, kAuth);
    // 2. if the token is saved in local
    if (token.length() == USER_TOKEN_LENGTH) {
        _user = new (nothrow) User(document);
        _user->loadUserInfo(_deviceToken, success, [=](int code) {
            if (InvalidTokenErrorCode == code) {
                CC_SAFE_DELETE(_user);
                requestLogin(success, failed);
            } else {
                MessageBoxLayer::getInstance()->show(LocalHelper::getString("networkResponse_loadUserInfoFailed"));
            }
        });
    } else {
//        CCASSERT(token.empty(), "The token is interrupted.");
        requestLogin(success, failed);
    }
}

bool GameData::isLoggedIn() const
{
    return _user != nullptr;
}

void GameData::generateUUID()
{
    static const char* key("uuid_key");
    UserDefault *ud = UserDefault::getInstance();
    _uuid = ud->getIntegerForKey(key, 0);
    if (0 == _uuid) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        const string& uuid = iOSApi::getUUID();
        _uuid = Utils::bkdrHash(uuid.c_str());
#else
        random_device rd;
        mt19937 mt(rd());
        _uuid = mt() & 0x7FFFFFFF;
#endif
        ud->setIntegerForKey(key, _uuid);
        ud->flush();
    }
    
    _uuName = Utils::toString(_uuid);
}

void GameData::requestLogin(const httpRequestCallback& success, const httpErrorCallback& failed)
{
    if (!isLoggedIn()) {
        NetworkApi::login(_uuName, [=](long code, const rapidjson::Value& jsonDict) {
            if (HttpSuccessCode == code) {
                _user = new (nothrow) User(jsonDict);
                saveAnonymousUser(_user);
                _user->loadUserInfo(_deviceToken, success, failed);
            } else if (failed) {
                failed(InvalidTokenErrorCode);
            }
        });
    }
}

void GameData::reloadGame()
{
    // 1. reset memory
    Director::getInstance()->getScheduler()->unschedule(heartbeatScheduleKey, this);
    NetworkController::purge();
    CC_SAFE_DELETE(_user);
    
    // 2. reload
    init();
    
    // 3. update UI
    auto director = Director::getInstance();
    auto scene = MainScene::create();
    director->purgeCachedData();
    director->popToRootScene();
    director->replaceScene(scene);
}

#pragma mark - user info
void GameData::loadAnonymousUser(rapidjson::Document& document)
{
#if false
    iOSApi::loadAnonymousUser(document);
#else
    UserDefault *ud = UserDefault::getInstance();
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember(kUID, ud->getIntegerForKey(kUID), allocator);
    document.AddMember(kAuth, rapidjson::Value(ud->getStringForKey(kAuth).c_str(), allocator), allocator);
#endif
}

void GameData::saveAnonymousUser(const User* user)
{
#if false
    iOSApi::saveAnonymousUser(_user);
#else
    UserDefault *ud = UserDefault::getInstance();
    ud->setIntegerForKey(kUID, user->getUid());
    ud->setStringForKey(kAuth, user->getToken());
    ud->flush();
#endif
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
    NetworkApi::heartBeat([this](long, const rapidjson::Value& jsonDict) {
        auto online = JSonUtils::parse<bool>(jsonDict, "online");
        if (!online) {
            onUserIsOffline();
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
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
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
        #if false
    MessageBox(StringUtils::format(LocalHelper::getString("networkResponse_errorCode").c_str(), code, jsonPath.c_str()).c_str(), LocalHelper::getString("networkResponse_errorTitle").c_str());
        #endif
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
#if false
    MessageBoxLayer::getInstance()->show(LocalHelper::getString("networkResponse_isOffline"), MessageBoxType::OnlyYes, [this](Ref *) {
        reloadGame();
    });
#endif
}
