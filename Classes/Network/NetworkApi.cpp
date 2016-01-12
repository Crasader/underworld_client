//
//  NetworkApi.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "NetworkApi.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "MD5Verifier.h"
#include "GameData.h"
#include "NetworkController.h"
#include "ProgressLayer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#include "TargetConditionals.h"
#endif

using namespace std;
USING_NS_CC;
using namespace cocos2d::network;

#if COCOS2D_DEBUG
static const string kApiHost = "http://192.168.31.222:8080/";
#else
static const string kApiHost = "http://203.88.170.234/";
#endif

static const string kAuthHeader = "X-Auth-Token";
static const string kContentType = "application/x-www-form-urlencoded";
static const string kSignHeader = "X-Auth-Signature";
static const string kSignParam = "sign";

static const string kServerPrefix = "Underworld_Server/";

static const string kGuestLogin         = kServerPrefix + "guest.json";
static const string kUserInfo           = kServerPrefix + "user/info.json";
static const string kHeartBeat          = kServerPrefix + "user/online.json";
static const string kDailyQuestProgress = kServerPrefix + "quest/dailyList.json";
static const string kLifeQuestProgress  = kServerPrefix + "quest/lifeList.json";
static const string kFinishQuest        = kServerPrefix + "quest/finish.json";
static const string kiOSIAP             = kServerPrefix + "iosiap.json";

// ======================= Hero =======================
static const string kGetHeroesList      = kServerPrefix + "hero/list.json";
static const string kGetHeroDetail      = kServerPrefix + "hero/detail.json";
static const string kUpgradeHero        = kServerPrefix + "hero/upgrade.json";
static const string kSetArtifact        = kServerPrefix + "hero/equip.json";
static const string kUpgradeArtifact    = kServerPrefix + "hero/upgradeEquip.json";
static const string kSellArtifact       = kServerPrefix + "hero/decomposeEquip.json";
static const string kComposeHero        = kServerPrefix + "hero/compose.json";
static const string kSellHeroPiece      = kServerPrefix + "hero/sellPiece.json";

// ======================= Soldier =======================
static const string kGetSoldiersList    = kServerPrefix + "soldier/list.json";
static const string kGetSoldierDetail   = kServerPrefix + "soldier/detail.json";
static const string kUpgradeSoldier     = kServerPrefix + "soldier/upgrade.json";
static const string kRecastSoldier      = kServerPrefix + "soldier/evolve.json";
static const string kSwitchSoldierTalent= kServerPrefix + "soldier/switchSkill.json";
static const string kSetGear            = kServerPrefix + "soldier/equip.json";
static const string kUpgradeGear        = kServerPrefix + "soldier/upgradeEquip.json";
static const string kSellGear           = kServerPrefix + "soldier/decomposeEquip.json";
static const string kComposeSoldier     = kServerPrefix + "soldier/compose.json";
static const string kSellSoldierPiece   = kServerPrefix + "soldier/sellPiece.json";

// ======================= Tower =======================
static const string kGetTowersList      = kServerPrefix + "building/.json";
static const string kGetTowerDetail     = kServerPrefix + "building/.json";
static const string kUpgradeTower       = kServerPrefix + "building/.json";

static string headerString(const string& key, const string& value)
{
    return StringUtils::format("%s:%s", key.c_str(), value.c_str());
}

static string queryString(const map<string, string>& params, const string& key)
{
    string result;
    string digestData;
    for (map<string, string>::const_iterator iter = params.begin(); iter != params.end(); ++iter)
    {
        if (result.length() == 0)
        {
            result = StringUtils::format("%s=%s", iter->first.c_str(), CocosUtils::urlEncode(iter->second).c_str());
            digestData.append(iter->first + "=" + iter->second);
        }
        else
        {
            result = StringUtils::format("%s&%s=%s", result.c_str(), iter->first.c_str(), CocosUtils::urlEncode(iter->second).c_str());
            digestData.append("&" + iter->first + "=" + iter->second);
        }
    }
    if(result.length() > 0) {
        result.append("&");
    }
    result.append(kSignParam + "=" + MD5Verifier::getInstance()->digest(digestData, key));
    return result;
}

static string responseDataToString(const vector<char>* data)
{
    string dataString;
    dataString.assign(data->begin(), data->end());
    return dataString;
}

static void request(const string& path,
                    const ccHttpRequestCallback& callback,
                    const map<string, string> * params = nullptr,
                    HttpRequest::Type type = HttpRequest::Type::POST,
                    bool isLocalTest = false,
                    bool showLoadingView = true,
                    bool isImmediate = true,
                    int timeout = TimeoutDuration)
{
    if (isLocalTest)
    {
        string fullPath = StringUtils::format("fake_network_response/%s", path.c_str());
        FileUtils *fileUtils = FileUtils::getInstance();
        if (fileUtils->isFileExist(fullPath))
        {
            // ignore all parameters, only return a fake "HttpResponse"
            HttpResponse *fakeResponse = new (nothrow) HttpResponse(nullptr);
            
            string content = fileUtils->getStringFromFile(fullPath);
            vector<char> *data = new (nothrow) vector<char>;
            data->assign(content.begin(), content.end());
            fakeResponse->setResponseData(data);
            fakeResponse->setSucceed(true);
            fakeResponse->setResponseCode(HttpSuccessCode);
            delete data;
            
            callback(nullptr, fakeResponse);
            delete fakeResponse;
        }
        else
        {
            CCLOG("Local network request failed, \"%s\" not exist.", path.c_str());
            HttpResponse *fakeResponse = new (nothrow) HttpResponse(nullptr);
            fakeResponse->setSucceed(true);
            fakeResponse->setResponseCode(HttpSuccessCode);
            
            callback(nullptr, fakeResponse);
            delete fakeResponse;
        }
    }
    else
    {
        HttpRequest* request = new (nothrow) HttpRequest();
        request->setUrl(StringUtils::format("%s%s", kApiHost.c_str(), path.c_str()).c_str());
        request->setRequestType(type);
        
        string token;
        // Headers
        vector<string> headers;
        headers.push_back(headerString("Content-Type", kContentType));
        if (GameData::getInstance()->isLoggedIn() && type == HttpRequest::Type::POST)
        {
            // Token
#if TARGET_IPHONE_SIMULATOR
            token = GameData::getInstance()->currentUser()->getToken();
#else
            token = GameData::getInstance()->currentUser()->getToken();
#endif
            headers.push_back(headerString(kAuthHeader, token));
        }
        request->setHeaders(headers);
        
        // Data
        map<string, string> fullParams;
        // 1. device
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        fullParams.insert(make_pair("device", "1"));
#else
        fullParams.insert(make_pair("device", "2"));
#endif  // CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        
        // 2. version
        string versionId = GameData::getInstance()->getVersionId();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        versionId += "b" + iOSApi::getBuildId();
#else
#endif
        fullParams.insert(make_pair("version",  versionId));
        
        // 3. other data
        if (nullptr != params) {
            for (map<string, string>::const_iterator iter = params->begin(); iter != params->end(); ++iter) {
                fullParams.insert(make_pair(iter->first, iter->second));
            }
        }
        
        string dataString = queryString(fullParams, token);
        
        if (dataString.length() > 0)
        {
            request->setRequestData(dataString.c_str(), dataString.length());
        }
        
        // Callback
        request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
            if (showLoadingView)
            {
                NetworkController::getInstance()->popRequest();
                if (NetworkController::getInstance()->isEmpty())
                {
                    ProgressLayer::hideSingleton();
                }
            }
            
            const long code = response->getResponseCode();
            GameData *gameData = GameData::getInstance();
            
            if (code == OfflineCode)
            {
                gameData->onUserIsOffline();
            }
            else if (code == VersionErrorCode)
            {
                gameData->onVersionError();
            }
            else
            {
                if (token.length() > 0)
                {
                    gameData->onUserIsOnline();
                }
                
                if (code != HttpSuccessCode)
                {
                    gameData->onNetworkResponseFailed(code, path);
                }
                
                if (callback)
                {
                    callback(client, response);
                }
            }
        });
        
        // Timeout
        HttpClient::getInstance()->setTimeoutForConnect(timeout);
        
        if (isImmediate)
        {
            string tag = StringUtils::format("%d isImmediate : %s", type, path.c_str());
            request->setTag(tag.c_str());
            HttpClient::getInstance()->sendImmediate(request);
        }
        else
        {
            string tag = StringUtils::format("%d : %s", type, path.c_str());
            request->setTag(tag.c_str());
            HttpClient::getInstance()->send(request);
        }
        
        if (showLoadingView)
        {
            if (NetworkController::getInstance()->isEmpty())
            {
                ProgressLayer::showSingleton();
            }
            NetworkController::getInstance()->pushRequest(request);
        }
        
        request->release();
    }
}

void NetworkApi::parseResponseData(const vector<char>* responseData, rapidjson::Document& jsonDict)
{
    string data = responseDataToString(responseData);
    jsonDict.Parse<0>(data.c_str());
    if (jsonDict.HasParseError())
    {
        CCLOG("GetParseError %d\n",jsonDict.GetParseError());
    }
}

void NetworkApi::login(const ccHttpRequestCallback& callback)
{
    request(kGuestLogin, callback);
}

void NetworkApi::loadUserInfo(const string& deviceToken, const ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("iostoken", deviceToken));
    request(kUserInfo, callback, &params);
}

void NetworkApi::heartBeat(const ccHttpRequestCallback& callback, bool showLoadingView)
{
    request(kHeartBeat, callback, nullptr, HttpRequest::Type::POST, false, showLoadingView);
}

#pragma mark - Quest
void NetworkApi::getDailyQuestProgress(const ccHttpRequestCallback& callback)
{
    request(kDailyQuestProgress, callback);
}

void NetworkApi::getLifeQuestProgress(const ccHttpRequestCallback& callback)
{
    request(kLifeQuestProgress, callback);
}

void NetworkApi::finishQuest(QuestType type, int questId, const ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("type", StringUtils::format("%d", type)));
    params.insert(make_pair("qid", StringUtils::format("%d", questId)));
    request(kFinishQuest, callback, &params);
}

#pragma mark - Guild


#pragma mark - Hero
void NetworkApi::getHeroesList(const cocos2d::network::ccHttpRequestCallback& callback)
{
    request(kGetHeroesList, callback);
}

void NetworkApi::getHeroDetail(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", id)));
    request(kGetHeroDetail, callback, &params);
}

void NetworkApi::upgradeHero(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", id)));
    request(kUpgradeHero, callback, &params);
}

void NetworkApi::setArtifact(int heroId, int artifactId, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("suid", StringUtils::format("%d", heroId)));
    params.insert(make_pair("euid", StringUtils::format("%d", artifactId)));
    request(kSetArtifact, callback, &params);
}

void NetworkApi::upgradeArtifact(int heroId, int artifactId, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", heroId)));
    params.insert(make_pair("index", StringUtils::format("%d", artifactId)));
    request(kUpgradeArtifact, callback, &params);
}

void NetworkApi::sellArtifact(int artifactId, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("euid", StringUtils::format("%d", artifactId)));
    request(kSellArtifact, callback, &params);
}

void NetworkApi::composeHero(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("puid", StringUtils::format("%d", id)));
    request(kComposeHero, callback, &params);
}

void NetworkApi::sellHeroPiece(int id, int count, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("puid", StringUtils::format("%d", id)));
    params.insert(make_pair("amount", StringUtils::format("%d", count)));
    request(kSellHeroPiece, callback, &params);
}

#pragma mark - Soldier
void NetworkApi::getSoldiersList(const cocos2d::network::ccHttpRequestCallback& callback)
{
    request(kGetSoldiersList, callback);
}

void NetworkApi::getSoldierDetail(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", id)));
    request(kGetSoldierDetail, callback, &params);
}

void NetworkApi::upgradeSoldier(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", id)));
    request(kUpgradeSoldier, callback, &params);
}

void NetworkApi::recastSoldier(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", id)));
    request(kRecastSoldier, callback, &params);
}

void NetworkApi::switchSoldierTalent(int soldierId, int talentId, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", soldierId)));
    params.insert(make_pair("index", StringUtils::format("%d", talentId)));
    request(kSwitchSoldierTalent, callback, &params);
}

void NetworkApi::setGear(int soldierId, int gearId, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("suid", StringUtils::format("%d", soldierId)));
    params.insert(make_pair("euid", StringUtils::format("%d", gearId)));
    request(kSetGear, callback, &params);
}

void NetworkApi::upgradeGear(int gearId, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("euid", StringUtils::format("%d", gearId)));
    request(kUpgradeGear, callback, &params);
}

void NetworkApi::sellGear(int gearId, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("euid", StringUtils::format("%d", gearId)));
    request(kSellGear, callback, &params);
}

void NetworkApi::composeSoldier(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("puid", StringUtils::format("%d", id)));
    request(kComposeSoldier, callback, &params);
}

void NetworkApi::sellSoldierPiece(int id, int count, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("puid", StringUtils::format("%d", id)));
    params.insert(make_pair("amount", StringUtils::format("%d", count)));
    request(kSellSoldierPiece, callback, &params);
}

#pragma mark - Tower
void NetworkApi::getTowersList(const cocos2d::network::ccHttpRequestCallback& callback)
{
    request(kGetTowersList, callback);
}

void NetworkApi::getTowerDetail(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", id)));
    request(kGetTowerDetail, callback, &params);
}

void NetworkApi::upgradeTower(int id, const cocos2d::network::ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("id", StringUtils::format("%d", id)));
    request(kUpgradeTower, callback, &params);
}

#pragma mark - IAP
void NetworkApi::iap(bool isSandBox, const string& receiptData, const ccHttpRequestCallback& callback)
{
    map<string, string> params;
    params.insert(make_pair("receiptData", receiptData));
    params.insert(make_pair("sandbox", isSandBox ? "1" : "0"));
    request(kiOSIAP, callback, &params, HttpRequest::Type::POST, false, false, true, 900);
}