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
    typedef const std::function<void(long, const rapidjson::Value&)> HttpCallback;
    
    // ======================= Game =======================
    void login(const HttpCallback& callback);
    void loadUserInfo(const std::string& deviceToken, const HttpCallback& callback);
    void heartBeat(const HttpCallback& callback, bool showLoadingView);
    
    // ======================= Quest =======================
    void getDailyQuestProgress(const HttpCallback& callback);
    void getLifeQuestProgress(const HttpCallback& callback);
    void finishQuest(QuestType type, int questId, const HttpCallback& callback);
    
    // ======================= Guild =======================
    
    
    // ======================= Card =======================
    void getCardList(const HttpCallback& callback);
    void getCardDetail(int cardDbId, const HttpCallback& callback);
    void upgradeCard(int cardDbId, int level, const HttpCallback& callback);
    void upgradeCardSkill(int cardDbId, int level, int skillIdx, const HttpCallback& callback);
    void getRunesList(const HttpCallback& callback);
    void imbedRune(int cardDbId, int runeIdx, int runeDbId, const HttpCallback& callback);
    void unloadRune(int cardDbId, int runeIdx, const HttpCallback& callback);
    void upgradeRune(int runeDbId, int level, const HttpCallback& callback);
    void compoundRune(int runeDbId, const HttpCallback& callback);
    
    // ======================= Chat =======================
    void sendMessage(ChatType type, int contactor, const std::string& message, const HttpCallback& callback);
    void recieveMessages(const HttpCallback& callback);
    
    // ======================= IAP =======================
    void iap(bool isSandBox, const std::string& receiptData, const HttpCallback& callback);
}

#endif /* NetworkApi_h */
