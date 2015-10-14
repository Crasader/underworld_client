//
//  GameData.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GameData_h
#define GameData_h

#include "User.h"
#include "CocosGlobal.h"

class IapObject;

typedef std::function<void()> FetchIAPInfoCallback;

class GameData
{
public:
    static GameData * getInstance();
    static void purge();
    
    void init() const;
    
    User* currentUser() const;
    
    void setDeviceToken(const string& token);
    const string& getVersionId() const;
    
    // auto login (if a token is exist in local)
    void autoLogin(const httpRequestCallback& success, const httpRequestCallback& failed);
    
    // guest account
    void login(const httpRequestCallback& success, const httpRequestCallback& failed);
    bool isLoggedIn() const;
    
    // iap
    bool isTransacting() const;
    void beginTransaction();
    void finishTransaction();
    void fetchIAPInfo(const FetchIAPInfoCallback& success, const FetchIAPInfoCallback& failed, bool showLoadingLayer);
    const vector<IapObject *>& getIapObjects() const;
    
    // heartbeat
    void startHeartBeatSynchronously();
    void scheduleHeartBeat();
    void heartBeat(float dt);
    void heartBeatRequest(bool showLoadingView);
    void stopHeartBeat();
    
    // callbacks
    void onLogout();
    void onUpdateVersionID(const string& version);
    void onNetworkResponseFailed(int code, const string& jsonPath) const;
    void onVersionError();
    void onUserIsOnline();
    void onUserIsOffline();
    
private:
    GameData();
    virtual ~GameData();
    
    // login
    void requestLogin(const httpRequestCallback& success, const httpRequestCallback& failed);
    void handleLoginResponse(cocos2d::network::HttpResponse* response, const httpRequestCallback& success, const httpRequestCallback& failed);
    void reloadGame();
    
private:
    User *_user;
    string _versionId;
    string _deviceToken;
    bool _isTransacting;
    vector<IapObject *> _iapObjects;
};

#endif /* GameData_h */
