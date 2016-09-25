//
//  GameData.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GameData_h
#define GameData_h

#include "network/HttpClient.h"
#include "User.h"
#include "CocosGlobal.h"

class GameData
{
public:
    static GameData * getInstance();
    static void purge();
    
    void init() const;
    void parseFiles() const;
    
    User* currentUser() const;
    
    void setDeviceToken(const std::string& token);
    int getUUID() const;
    const std::string& getVersionId() const;
    
    // auto login (if a token is exist in local)
    void autoLogin(const httpRequestCallback& success, const httpErrorCallback& failed);
    
    // guest account
    void login(const httpRequestCallback& success, const httpErrorCallback& failed);
    bool isLoggedIn() const;
    
    // heartbeat
    void startHeartBeatSynchronously();
    void scheduleHeartBeat();
    void heartBeat(float dt);
    void heartBeatRequest(bool showLoadingView);
    void stopHeartBeat();
    
    // callbacks
    void onLogout();
    void onUpdateVersionID(const std::string& version);
    void onNetworkResponseFailed(long code, const std::string& jsonPath) const;
    void onVersionError();
    void onUserIsOnline();
    void onUserIsOffline();
    
private:
    GameData();
    virtual ~GameData();
    
    void generateUUID();
    // login
    void requestLogin(const httpRequestCallback& success, const httpErrorCallback& failed);
    void reloadGame();
    
    // user info
    void loadAnonymousUser(rapidjson::Document& document);
    void saveAnonymousUser(const User* user);
    
private:
    User *_user;
    int _uuid;
    std::string _uuName;
    std::string _versionId;
    std::string _deviceToken;
};

#endif /* GameData_h */
