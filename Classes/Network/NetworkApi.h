//
//  NetworkApi.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef NetworkApi_h
#define NetworkApi_h

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "json/document.h"
#include "CocosGlobal.h"

#define TimeoutDuration     (10)

#define HttpSuccessCode         (200)
#define InvalidTokenErrorCode   (401)
#define VersionErrorCode        (888)
#define OfflineCode             (999)

namespace NetworkApi
{
    // basic    
    inline bool isSuccessfulResponse(cocos2d::network::HttpResponse* response) { return (response->isSucceed() && response->getResponseCode() == HttpSuccessCode); }
    
    void parseResponseData(const std::vector<char>* responseData, rapidjson::Document& jsonDict);
    
    // ======================= Game =======================
    void login(const cocos2d::network::ccHttpRequestCallback& callback);
    
    void loadUserInfo(const std::string& deviceToken, const cocos2d::network::ccHttpRequestCallback& callback);
    
    void heartBeat(const cocos2d::network::ccHttpRequestCallback& callback, bool showLoadingView);
    
    // ======================= Quest =======================
    void getDailyQuestProgress(const cocos2d::network::ccHttpRequestCallback& callback);
    void getLifeQuestProgress(const cocos2d::network::ccHttpRequestCallback& callback);
    void finishQuest(QuestType type, int questId, const cocos2d::network::ccHttpRequestCallback& callback);
    
    // ======================= Guild =======================
    
    
    // ======================= IAP =======================
    void iap(bool isSandBox, const std::string& receiptData, const cocos2d::network::ccHttpRequestCallback& callback);
}

#endif /* NetworkApi_h */
