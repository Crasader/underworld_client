//
//  GameManager.h
//  Underworld_Client
//
//  Created by Andy on 16/7/18.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GameManager_h
#define GameManager_h

#include "cocos2d.h"
#include "ExternalInterface.h"
#include "ClientTCPNetwork.h"

namespace UnderWorld { namespace Core {
    class GameLooper;
    class GameSettings;
    class GameModeHMMSetting;
}}

class GameScheduler;
class ClientTCPNetwork;

enum GameMode {
    kPvp,
    kPve,
    GAME_MODE_COUNT,
};

enum GameState {
    kIdle,
    kLaunchingPVR,
    kLaunchingNetwork,
    kPlaying,
    GAME_STATE_COUNT
};


class GameClient : public ClientTCPNetwork::LaunchListener,
    public UnderWorld::Core::AbstractRender::RenderListener
{
public:
    GameClient();
    virtual ~GameClient();
    
    // interface
    void launchPve(int mapId);
    void launchPvp();
    void cancelLaunch();
    void exitGame();
    
private:
    
    
    // private
    void launchGame();
    
    void restartPve();
    
    void createInstance();
    void purgeInstance();
    bool loadBattleContent();
    void resetStatus();
    
    void onPVRLoaded();
    
    void loadTechTree2Settings();
    void loadMap2Settings(int mapId);
    
    // override ClientTCPNetwork::LaunchListener
    virtual void onLaunched(const NetworkMessageLaunch2C& l2c) override;
    
    // override AbstractRender::RenderListener
    virtual void onExitRequest() override;
    virtual bool onPauseRequest() override;
    virtual bool onResumeRequest() override;

public:
    static void loadCommonMapSetting(int mapId, UnderWorld::Core::MapSetting& output);
    static void loadHMMMapSetting(int mapId, std::string& output);
    static void loadCommonTechTree(std::string& output);
    static void loadHMMMTechTree(std::string& output);

    
private:
    struct BattleContent;
    
    // status
    int _mapId;
    GameMode _mode;
    GameState _state;
    
    // instance
    cocos2d::Scene* _scene;
    UnderWorld::Core::AbstractRender* _render;
    GameScheduler* _gameScheduler;
    ClientTCPNetwork* _network;
    UnderWorld::Core::GameLooper* _looper;
    BattleContent* _battleContent;
    UnderWorld::Core::GameSettings* _settings;
    UnderWorld::Core::GameModeHMMSetting* _hmmSettings;
};

class GameManager {
public:
    static GameClient* getGameClient();
    static void purgeGameClient();
private:
    GameManager();
};

#endif /* GameManager_h */
