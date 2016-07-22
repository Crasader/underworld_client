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
#include "GameModeHMM.h"

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
    UnderWorld::Core::GameModeHMMSetting _hmmSetting;
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
    virtual ~UnderworldClient();
    
    /** interface */
    void launchPvp(const UnderWorld::Core::GameContentSetting& setting,
        const std::vector<std::string>& cards,
        const UnderWorld::Core::GameModeHMMSetting::InitUnitList& initUnits,
        const std::vector<UnderWorld::Core::UnitSetting>& unitPool);
    void launchPve(int map, const UnderWorld::Core::GameContentSetting& setting,
        const std::vector<std::string>& cards,
        const UnderWorld::Core::GameModeHMMSetting::InitUnitList& initList,
        const std::vector<UnderWorld::Core::UnitSetting>& unitPool);
    void cancelLaunch();
    void quit();
    
    /** override */
    virtual void onReceive(const std::vector<UnderWorld::Core::NetworkMessage*>& msgs);
    
private:
    void loadTechTree();
    void loadMap(int mapId);
    
public:
    static void loadCommonMapSetting(int mapId, UnderWorld::Core::MapSetting& output);
    static void loadHMMMapSetting(int mapId, std::string& output);
    static void loadCommonTechTree(std::string& output);
    static void loadHMMMTechTree(std::string& output);
    
};
    
class NetworkMessageLaunch2S : public UnderWorld::Core::NetworkMessage {
private:
    typedef UnderWorld::Core::GameContentSetting ContentSetting;
    typedef UnderWorld::Core::GameModeHMMSetting::InitUnitList InitUnits;
    typedef std::vector<UnderWorld::Core::UnitSetting> UnitPool;
    
private:
    ContentSetting _setting;
    std::vector<std::string> _cards;
    InitUnits _initUnits;
    UnitPool _unitPool;
    
public:
    NetworkMessage* clone() const override                     {return new NetworkMessageLaunch2S(*this);}
    
    /** getters */
    const ContentSetting& getGameContentSetting() const        {return _setting;}
    const std::vector<std::string>& getCards() const           {return _cards;}
    const InitUnits& getInitUnits() const                      {return _initUnits;}
    const UnitPool& getUnitPool() const                        {return _unitPool;}
    
    /** setters */
    void setGameContentSetting(const ContentSetting& setting)  {_setting = setting;}
    void setCards(const std::vector<std::string>& cards)       {_cards = cards;}
    void setInitUnits(const InitUnits& initUnits)              {_initUnits = initUnits;}
    void setUnitPool(const UnitPool& unitPool)                 {_unitPool = unitPool;}
};

class NetworkMessageLaunch2C : public UnderWorld::Core::NetworkMessage {
private:
    typedef UnderWorld::Core::FactionSetting FactionSetting;
    typedef std::vector<std::vector<std::string> > Cards;
    typedef std::vector<UnderWorld::Core::GameModeHMMSetting::InitUnitList> InitUnitLists;
    typedef std::vector<std::vector<UnderWorld::Core::UnitSetting> > UnitPools;
private:
    FactionSetting _factionSetting;
    Cards _cards;
    InitUnitLists _initUnitLists;
    UnitPools _unitPools;
    int _mapId;
    
public:
    NetworkMessage* clone() const override                 {return new NetworkMessageLaunch2C(*this);}
    const FactionSetting& getFactionSetting() const        {return _factionSetting;}
    const Cards& getCards() const                          {return _cards;}
    const InitUnitLists& getInitUnitLists() const          {return _initUnitLists;}
    const UnitPools& getUnitPools() const                  {return _unitPools;}
    int getMapId() const                                   {return _mapId;}
    void setFactionSetting(const FactionSetting& fs)       {_factionSetting = fs;}
    void setCards(const Cards& cards)                      {_cards = cards;}
    void setInitUnitLists(const InitUnitLists& list)       {_initUnitLists = list;}
    void setUnitPools(const UnitPools& pools)              {_unitPools = pools;}
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
