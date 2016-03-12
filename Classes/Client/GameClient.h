//
//  GameClient.h
//  Underworld_Client
//
//  Created by wenchengye on 16/3/12.
//
//

#ifndef GameClient_h
#define GameClient_h

#include "GameLooper.h"
#include "NetworkServiceImpl.h"
#include "GameSettings.h"

namespace UnderWorld{ namespace Core{
    
enum GameMode {
    kPvp,
    kPve,
    GAME_MODE_COUNT,
};
    
enum GameState {
    kIdle,
    kLaunching,
    kPlaying,
    GAME_STATE_COUNT
};
    
class GameClient {
private:
    static const std::string SCHEDULE_KEY_CLIENT_LAUNCHING;
    static const int CLIENT_LAUNCHING_FPS = 10;
private:
    GameLooper _looper;
    ClientNetworkSerivce _net;
    GameSettings _settings;
    GameMode _mode;
    GameState _state;
    
    AbstractScheduler* _schduler;
    AbstractNetworkProxy* _proxy;
    
public:
    GameClient(AbstractRender* render,
        AbstractScheduler* scheduler,
        AbstractNetworkProxy* proxy);
    
    /** interface */
    void launchPvp(const GameContentSetting& setting);
    void launchPve(int map, const GameContentSetting& setting);
    void launchPve(const MapSetting& mapSetting,
        const GameContentSetting& setting);
    void cancelLaunch();
    void quit();
    
private:
    void updateLaunching();
    
    void loadTechTree();
    void loadMap(int mapId);
    void loadMap(const MapSetting& mapSetting);
    
};
    
    
}}


#endif /* GameClient_h */
