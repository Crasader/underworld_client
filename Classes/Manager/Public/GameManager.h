//
//  GameManager.h
//  Underworld_Client
//
//  Created by Andy on 16/7/18.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GameManager_h
#define GameManager_h

#include "GameRender.h"

class UnderworldClient;
class GameScheduler;
class ClientTCPNetworkProxy;

class GameManager : public GameRenderObserver
{
public:
    static GameManager* getInstance();
    static void purge();
    
    // pve
    void launchPve(int mapId);
    
    // pvp
    void launchPvp();
    
private:
    GameManager();
    virtual ~GameManager();
    
    // GameRenderObserver
    virtual void onGameRenderRestart() override;
    virtual void onGameRenderExit() override;
    
    // private
    void launchGame();
    void exitGame();
    void restartPve();
    
    void createClient(cocos2d::Scene* scene);
    void purgeClient();
    bool loadBattleContent();
    
private:
    struct BattleContent;
    
    bool _isPvp;
    int _mapId;
    cocos2d::Scene* _scene;
    GameRender* _render;
    GameScheduler* _gameScheduler;
    ClientTCPNetworkProxy* _pvpProxy;
    UnderworldClient* _client;
    BattleContent* _battleContent;
};

#endif /* GameManager_h */
