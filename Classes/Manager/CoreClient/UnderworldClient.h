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
#include "ClientTCPNetworkProxy.h"
#include "GameScheduler.h"
#include "NetworkMessage.h"
#include "GameSettings.h"

#define UNDER_WORLD_HOST ("")
#define UNDER_WORLE_PORT (8080)
    
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
    
class UnderworldClient : public UnderWorld::Core::AbstractNetworkProxy::NetworkProxyListener {
private:
    UnderWorld::Core::ClientTerminal _terminal;
    UnderWorld::Core::GameSettings _settings;
    GameMode _mode;
    GameState _state;
    std::string _name;
    
    UnderWorld::Core::AbstractNetworkProxy* _proxy;
    UnderWorld::Core::AbstractScheduler*  _scheduler;
    UnderWorld::Core::AbstractRender* _render;
    
public:
    UnderworldClient(const std::string& name,
        UnderWorld::Core::AbstractNetworkProxy* proxy,
        UnderWorld::Core::AbstractScheduler*  scheduler,
        UnderWorld::Core::AbstractRender* render);
    
    /** interface */
    void launchPvp(const UnderWorld::Core::GameContentSetting& setting);
    void launchPve(int map, const UnderWorld::Core::GameContentSetting& setting);
    void launchPve(const UnderWorld::Core::MapSetting& mapSetting,
        const UnderWorld::Core::GameContentSetting& setting);
    void cancelLaunch();
    void quit();
    
    /** override */
    virtual void onReceive(const std::vector<UnderWorld::Core::NetworkMessage*>& msgs);
    
private:
    void loadTechTree();
    void loadMap(int mapId);
    void loadMap(const UnderWorld::Core::MapSetting& mapSetting);
    
};
    
class NetworkMessageLaunch2S : public UnderWorld::Core::NetworkMessage {
private:
    typedef UnderWorld::Core::GameContentSetting ContentSetting;
    
private:
    ContentSetting _setting;
    
public:
    NetworkMessage* clone() const override                     {return new NetworkMessageLaunch2S(*this);}
    const ContentSetting& getGameContentSetting() const        {return _setting;}
    void setGameContentSetting(const ContentSetting& setting)  {_setting = setting;}
};

class NetworkMessageLaunch2C : public UnderWorld::Core::NetworkMessage {
private:
    typedef UnderWorld::Core::FactionSetting FactionSetting;
private:
    FactionSetting _factionSetting;
    int _mapId;
    
public:
    NetworkMessage* clone() const override                 {return new NetworkMessageLaunch2C(*this);}
    const FactionSetting& getFactionSetting() const        {return _factionSetting;}
    int getMapId() const                                   {return _mapId;}
    void setFactionSetting(const FactionSetting& fs)       {_factionSetting = fs;}
    void setMapId(int mapId)                               {_mapId = mapId;}
};

class NetworkMessageCancel2S : public UnderWorld::Core::NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageCancel2S(*this);}

};

class NetworkMessageCancel2C : public UnderWorld::Core::NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageCancel2C(*this);}
};


#endif /* GameClient_h */
