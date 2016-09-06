//
//  NetworkApi.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "NetworkApi.h"
#include "CocosUtils.h"
#include "MD5Verifier.h"
#include "GameData.h"
#include "NetworkController.h"
#include "ProgressLayer.h"
#include "ChatMark.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#include "TargetConditionals.h"
#endif

using namespace std;
USING_NS_CC;
using namespace cocos2d::network;

#if COCOS2D_DEBUG
static const string kApiHost = "http://192.168.31.100:8080/";
#else
static const string kApiHost = "http://203.88.170.234/";
#endif

static const string kAuthHeader = "X-Auth-Token";
static const string kContentType = "application/x-www-form-urlencoded";
static const string kSignHeader = "X-Auth-Signature";
static const string kSignParam = "sign";

static const string kServerPrefix = "underworld_server/";

#pragma mark - private
namespace NetworkApi {
    void parseResponseData(const vector<char>* responseData, rapidjson::Document& jsonDict);
    string headerString(const string& key, const string& value);
    string queryString(const unordered_map<string, string>& params, const string& key);
    string responseDataToString(const vector<char>* data);
    void request(const string&, const ccHttpRequestCallback&, const unordered_map<string, string>*, HttpRequest::Type, bool, bool, bool, int);
    void request(const string&, const HttpCallback&, const unordered_map<string, string>*, HttpRequest::Type, bool, bool, bool, int);
}

string NetworkApi::headerString(const string& key, const string& value)
{
    return StringUtils::format("%s:%s", key.c_str(), value.c_str());
}

string NetworkApi::queryString(const unordered_map<string, string>& params, const string& key)
{
    string result;
    string digestData;
    for (auto iter = begin(params); iter != end(params); ++iter) {
        if (result.empty()) {
            result = StringUtils::format("%s=%s", iter->first.c_str(), CocosUtils::urlEncode(iter->second).c_str());
            digestData.append(iter->first + "=" + iter->second);
        } else {
            result = StringUtils::format("%s&%s=%s", result.c_str(), iter->first.c_str(), CocosUtils::urlEncode(iter->second).c_str());
            digestData.append("&" + iter->first + "=" + iter->second);
        }
    }
    if(!result.empty()) {
        result.append("&");
    }
    result.append(kSignParam + "=" + MD5Verifier::getInstance()->digest(digestData, key));
    return result;
}

string NetworkApi::responseDataToString(const vector<char>* data)
{
    string dataString;
    dataString.assign(data->begin(), data->end());
    return dataString;
}

void NetworkApi::request(const string& path,
                         const ccHttpRequestCallback& callback,
                         const unordered_map<string, string> * params,
                         HttpRequest::Type type,
                         bool isLocalTest,
                         bool showLoadingView,
                         bool isImmediate,
                         int timeout)
{
    if (isLocalTest) {
        auto fullPath = StringUtils::format("fake_network_response/%s", path.c_str());
        auto fileUtils = FileUtils::getInstance();
        if (fileUtils->isFileExist(fullPath)) {
            // ignore all parameters, only return a fake "HttpResponse"
            auto fakeResponse = new (nothrow) HttpResponse(nullptr);
            auto content = fileUtils->getStringFromFile(fullPath);
            auto data = new (nothrow) vector<char>;
            data->assign(content.begin(), content.end());
            fakeResponse->setResponseData(data);
            fakeResponse->setSucceed(true);
            fakeResponse->setResponseCode(HttpSuccessCode);
            delete data;
            
            callback(nullptr, fakeResponse);
            delete fakeResponse;
        } else {
            CCLOG("Local network request failed, \"%s\" not exist.", path.c_str());
            auto fakeResponse = new (nothrow) HttpResponse(nullptr);
            fakeResponse->setSucceed(true);
            fakeResponse->setResponseCode(HttpSuccessCode);
            
            callback(nullptr, fakeResponse);
            delete fakeResponse;
        }
    } else {
        auto request = new (nothrow) HttpRequest();
        request->setUrl(StringUtils::format("%s%s", kApiHost.c_str(), path.c_str()).c_str());
        request->setRequestType(type);
        
        string token;
        // Headers
        vector<string> headers;
        headers.push_back(headerString("Content-Type", kContentType));
        if (GameData::getInstance()->isLoggedIn() && type == HttpRequest::Type::POST) {
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
        unordered_map<string, string> fullParams;
        // 1. device
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        fullParams.insert(make_pair("device", "1"));
#else
        fullParams.insert(make_pair("device", "2"));
#endif  // CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        
        // 2. version
        auto versionId = GameData::getInstance()->getVersionId();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        versionId += "b" + iOSApi::getBuildId();
#else
#endif
        fullParams.insert(make_pair("version",  versionId));
        
        // 3. other data
        if (nullptr != params) {
            for (auto iter = params->begin(); iter != params->end(); ++iter) {
                fullParams.insert(make_pair(iter->first, iter->second));
            }
        }
        
        auto dataString = queryString(fullParams, token);
        
        if (dataString.length() > 0) {
            request->setRequestData(dataString.c_str(), dataString.length());
        }
        
        // Callback
        request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
            if (showLoadingView) {
                NetworkController::getInstance()->popRequest();
                if (NetworkController::getInstance()->isEmpty()) {
                    ProgressLayer::hideSingleton();
                }
            }
            
            const long code = response->getResponseCode();
            auto gameData = GameData::getInstance();
            
            if (code == OfflineCode) {
                gameData->onUserIsOffline();
            } else if (code == VersionErrorCode) {
                gameData->onVersionError();
            } else {
                if (token.length() > 0) {
                    gameData->onUserIsOnline();
                }
                
                if (code != HttpSuccessCode) {
                    gameData->onNetworkResponseFailed(code, path);
                }
                
                if (callback) {
                    callback(client, response);
                }
            }
        });
        
        // Timeout
        HttpClient::getInstance()->setTimeoutForConnect(10);
        HttpClient::getInstance()->setTimeoutForRead(timeout);
        
        if (isImmediate) {
            auto tag = StringUtils::format("%d isImmediate : %s", type, path.c_str());
            request->setTag(tag.c_str());
            HttpClient::getInstance()->sendImmediate(request);
        } else {
            auto tag = StringUtils::format("%d : %s", type, path.c_str());
            request->setTag(tag.c_str());
            HttpClient::getInstance()->send(request);
        }
        
        if (showLoadingView) {
            if (NetworkController::getInstance()->isEmpty()) {
                ProgressLayer::showSingleton();
            }
            NetworkController::getInstance()->pushRequest(request);
        }
        
        request->release();
    }
}

void NetworkApi::request(const string& path,
                         const HttpCallback& callback,
                         const unordered_map<string, string> * params = nullptr,
                         HttpRequest::Type type = HttpRequest::Type::POST,
                         bool isLocalTest = false,
                         bool showLoadingView = true,
                         bool isImmediate = true,
                         int timeout = TimeoutDuration)
{
    request(path, [callback](HttpClient* client, HttpResponse* response) {
        auto code(response->getResponseCode());
        rapidjson::Document jsonDict;
        if (response->isSucceed() && HttpSuccessCode == code) {
            parseResponseData(response->getResponseData(), jsonDict);
        }
        if (callback) {
            callback(code, jsonDict);
        }
    }, params, type, isLocalTest, showLoadingView, isImmediate, timeout);
}

void NetworkApi::parseResponseData(const vector<char>* responseData, rapidjson::Document& jsonDict)
{
    string data = responseDataToString(responseData);
    jsonDict.Parse<0>(data.c_str());
    if (jsonDict.HasParseError()) {
        CCLOG("GetParseError %d\n",jsonDict.GetParseError());
    }
}

#pragma mark - public
void NetworkApi::login(const HttpCallback& callback)
{
    static const string path(kServerPrefix + "guest.json");
    request(path, callback);
}

void NetworkApi::loadUserInfo(const string& deviceToken, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "user/info.json");
    unordered_map<string, string> params;
    params.insert(make_pair("iostoken", deviceToken));
    request(path, callback, &params);
}

void NetworkApi::heartBeat(const HttpCallback& callback, bool showLoadingView)
{
    static const string path(kServerPrefix + "user/online.json");
    request(path, callback, nullptr, HttpRequest::Type::POST, false, showLoadingView);
}

#pragma mark - Quest
void NetworkApi::getDailyQuestProgress(const HttpCallback& callback)
{
    static const string path(kServerPrefix + "quest/dailyList.json");
    request(path, callback);
}

void NetworkApi::getLifeQuestProgress(const HttpCallback& callback)
{
    static const string path(kServerPrefix + "quest/lifeList.json");
    request(path, callback);
}

void NetworkApi::finishQuest(QuestType type, int questId, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "quest/finish.json");
    unordered_map<string, string> params;
    params.insert(make_pair("type", StringUtils::format("%d", type)));
    params.insert(make_pair("qid", StringUtils::format("%d", questId)));
    request(path, callback, &params);
}

#pragma mark - Card
void NetworkApi::getCardList(const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/list.json");
    request(path, callback, nullptr);
}

void NetworkApi::getCardDetail(int cardDbId, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/detail.json");
    unordered_map<string, string> params;
    params.insert(make_pair("card", StringUtils::format("%d", cardDbId)));
    request(path, callback, &params);
}

void NetworkApi::upgradeCard(int cardDbId, int level, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/upgrade.json");
    unordered_map<string, string> params;
    params.insert(make_pair("card", StringUtils::format("%d", cardDbId)));
    params.insert(make_pair("level", StringUtils::format("%d", level)));
    request(path, callback, &params);
}

void NetworkApi::upgradeCardSkill(int cardDbId, int level, int skillIdx, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/upgradeskill.json");
    unordered_map<string, string> params;
    params.insert(make_pair("card", StringUtils::format("%d", cardDbId)));
    params.insert(make_pair("level", StringUtils::format("%d", level)));
    params.insert(make_pair("skill", StringUtils::format("%d", skillIdx)));
    request(path, callback, &params);
}

void NetworkApi::getRunesList(const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/runepacklist.json");
    request(path, callback, nullptr);
}

void NetworkApi::imbedRune(int cardDbId, int runeIdx, int runeDbId, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/imbedrune.json");
    unordered_map<string, string> params;
    params.insert(make_pair("card", StringUtils::format("%d", cardDbId)));
    params.insert(make_pair("runeindex", StringUtils::format("%d", runeIdx)));
    params.insert(make_pair("runepack", StringUtils::format("%d", runeDbId)));
    request(path, callback, &params);
}

void NetworkApi::unloadRune(int cardDbId, int runeIdx, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/removerune.json");
    unordered_map<string, string> params;
    params.insert(make_pair("card", StringUtils::format("%d", cardDbId)));
    params.insert(make_pair("runeindex", StringUtils::format("%d", runeIdx)));
    request(path, callback, &params);
}

void NetworkApi::upgradeRune(int runeDbId, int level, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/upgraderune.json");
    unordered_map<string, string> params;
    params.insert(make_pair("rune", StringUtils::format("%d", runeDbId)));
    params.insert(make_pair("level", StringUtils::format("%d", level)));
    request(path, callback, &params);
}

void NetworkApi::compoundRune(int runeDbId, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "card/compoundrune.json");
    unordered_map<string, string> params;
    params.insert(make_pair("runepack", StringUtils::format("%d", runeDbId)));
    request(path, callback, &params);
}

#pragma mark - Guild

#pragma mark - Chat
void NetworkApi::sendMessage(const ChatMark* mark, ChatType type, int contactor, const string& message, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "chat/send.json");
    unordered_map<string, string> params;
    mark->toMap(params);
    params.insert(make_pair("channel", StringUtils::format("%d", type)));
    params.insert(make_pair("to", StringUtils::format("%d", contactor)));
    params.insert(make_pair("content", message));
    request(path, callback, &params);
}

void NetworkApi::recieveMessages(const ChatMark* mark, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "chat/recieve.json");
    unordered_map<string, string> params;
    mark->toMap(params);
    request(path, callback, &params);
}

#pragma mark - IAP
void NetworkApi::iap(bool isSandBox, const string& receiptData, const HttpCallback& callback)
{
    static const string path(kServerPrefix + "iosiap.json");
    unordered_map<string, string> params;
    params.insert(make_pair("receiptData", receiptData));
    params.insert(make_pair("sandbox", isSandBox ? "1" : "0"));
    request(path, callback, &params, HttpRequest::Type::POST, false, false, true, 900);
}