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

class ChatMark;

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
    
    
    // ======================= Hero =======================
    void getHeroesList(const cocos2d::network::ccHttpRequestCallback& callback);
    void getHeroDetail(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void upgradeHero(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void setArtifact(int heroId, int artifactId, const cocos2d::network::ccHttpRequestCallback& callback);
    void upgradeArtifact(int heroId, int artifactId, const cocos2d::network::ccHttpRequestCallback& callback);
    void sellArtifact(int artifactId, const cocos2d::network::ccHttpRequestCallback& callback);
    void composeHero(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void sellHeroPiece(int id, int count, const cocos2d::network::ccHttpRequestCallback& callback);
    
    // ======================= Soldier =======================
    void getSoldiersList(const cocos2d::network::ccHttpRequestCallback& callback);
    void getSoldierDetail(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void upgradeSoldier(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void recastSoldier(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void switchSoldierTalent(int soldierId, int talentId, const cocos2d::network::ccHttpRequestCallback& callback);
    void setGear(int soldierId, int gearId, const cocos2d::network::ccHttpRequestCallback& callback);
    void upgradeGear(int gearId, const cocos2d::network::ccHttpRequestCallback& callback);
    void sellGear(int gearId, const cocos2d::network::ccHttpRequestCallback& callback);
    void composeSoldier(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void sellSoldierPiece(int id, int count, const cocos2d::network::ccHttpRequestCallback& callback);
    
    // ======================= Tower =======================
    void getTowersList(const cocos2d::network::ccHttpRequestCallback& callback);
    void getTowerDetail(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    void upgradeTower(int id, const cocos2d::network::ccHttpRequestCallback& callback);
    // ======================= Chat =======================
    void sendMessage(const ChatMark* mark, ChatType type, int contactor, const std::string& message, const cocos2d::network::ccHttpRequestCallback& callback);
    void recieveMessages(const ChatMark* mark, const cocos2d::network::ccHttpRequestCallback& callback);
    
    // ======================= IAP =======================
    void iap(bool isSandBox, const std::string& receiptData, const cocos2d::network::ccHttpRequestCallback& callback);
}

#endif /* NetworkApi_h */
