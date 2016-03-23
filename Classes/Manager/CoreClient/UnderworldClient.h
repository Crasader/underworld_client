//
//  GameClient.h
//  Underworld_Client
//
//  Created by wenchengye on 16/3/12.
//
//

#ifndef GameClient_h
#define GameClient_h

#include "ClientTerminal.h"
#include "ClientNetworkService.h"
#include "NetworkMessage.h"
#include "GameSettings.h"
    
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
    
class UnderworldClient {
private:
    static const std::string SCHEDULE_KEY_CLIENT_LAUNCHING;
    static const int CLIENT_LAUNCHING_FPS = 10;
private:
    UnderWorld::Core::ClientTerminal _terminal;
    UnderWorld::Core::ClientNetworkSerivce _net;
    UnderWorld::Core::GameSettings _settings;
    GameMode _mode;
    GameState _state;
    
    UnderWorld::Core::AbstractScheduler* _schduler;
    UnderWorld::Core::AbstractNetworkProxy* _proxy;
    
public:
    UnderworldClient(UnderWorld::Core::AbstractRender* render,
        UnderWorld::Core::AbstractScheduler* scheduler,
        UnderWorld::Core::AbstractNetworkProxy* proxy);
    
    /** interface */
    void launchPvp(const UnderWorld::Core::GameContentSetting& setting);
    void launchPve(int map, const UnderWorld::Core::GameContentSetting& setting);
    void launchPve(const UnderWorld::Core::MapSetting& mapSetting,
        const UnderWorld::Core::GameContentSetting& setting);
    void cancelLaunch();
    void quit();
    
private:
    void updateLaunching();
    
    void loadTechTree();
    void loadMap(int mapId);
    void loadMap(const UnderWorld::Core::MapSetting& mapSetting);
    
};
    
class NetworkMessageLaunch2S : public UnderWorld::Core::NetworkMessage {
private:
    UnderWorld::Core::GameContentSetting _setting;
    
public:
    const UnderWorld::Core::GameContentSetting& getGameContentSetting() const              {return _setting;}
    
    void setGameContentSetting(const UnderWorld::Core::GameContentSetting& setting)        {_setting = setting;}
};

class NetworkMessageLaunch2C : public UnderWorld::Core::NetworkMessage {
private:
    UnderWorld::Core::FactionSetting _factionSetting;
    int _mapId;
    
public:
    const UnderWorld::Core::FactionSetting& getFactionSetting() const        {return _factionSetting;}
    int getMapId() const                                   {return _mapId;}
    
    void setFactionSetting(const UnderWorld::Core::FactionSetting& fs)       {_factionSetting = fs;}
    void setMapId(int mapId)                               {_mapId = mapId;}
};

class NetworkMessageCancel2S : public UnderWorld::Core::NetworkMessage {
};

class NetworkMessageCancel2C : public UnderWorld::Core::NetworkMessage {
    
};


#endif /* GameClient_h */
