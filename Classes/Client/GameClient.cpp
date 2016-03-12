//
//  GameClient.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/3/12.
//
//

#include "GameClient.h"
#include "NetworkMessage.h"
#include "DataManager.h"

namespace UnderWorld{ namespace Core{
    
class NetworkMessageLaunch2S : public NetworkMessage {
private:
    GameContentSetting _setting;
    
public:
    virtual std::string toString() const override {return "";};
    virtual void fromString(const std::string& s) override {};
    
    const GameContentSetting& getGameContentSetting() const              {return _setting;}
    
    void setGameContentSetting(const GameContentSetting& setting)        {_setting = setting;}
};

class NetworkMessageLaunch2C : public NetworkMessage {
private:
    FactionSetting _factionSetting;
    int _mapId;
    
public:
    virtual std::string toString() const override {return "";};
    virtual void fromString(const std::string& s) override {};
    
    const FactionSetting& getFactionSetting() const        {return _factionSetting;}
    int getMapId() const                                   {return _mapId;}
    
    void setFactionSetting(const FactionSetting& fs)       {_factionSetting = fs;}
    void setMapId(int mapId)                               {_mapId = mapId;}
};

class NetworkMessageCancel2S : public NetworkMessage {
public:
    virtual std::string toString() const override {return "";};
    virtual void fromString(const std::string& s) override {};
    
};

class NetworkMessageCancel2C : public NetworkMessage {
public:
    virtual std::string toString() const override {return "";};
    virtual void fromString(const std::string& s) override {};
    
};

const std::string GameClient::SCHEDULE_KEY_CLIENT_LAUNCHING("schedule_key_client_launching");
    
GameClient::GameClient(AbstractRender* render,
    AbstractScheduler* scheduler, AbstractNetworkProxy* proxy)
: _looper(render, scheduler)
, _net(proxy)
, _mode(GAME_MODE_COUNT)
, _state(kIdle)
, _schduler(scheduler)
, _proxy(proxy) {
}
    
void GameClient::launchPvp(const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    
    _mode = kPvp;
    
    _proxy->connect();
    NetworkMessageLaunch2S* msg =
    new NetworkMessageLaunch2S();
    msg->setGameContentSetting(setting);
    _proxy->send(msg);
    _schduler->schedule(CLIENT_LAUNCHING_FPS,
        std::bind(&GameClient::updateLaunching, this),
        SCHEDULE_KEY_CLIENT_LAUNCHING);
    _state = kLaunching;
}
    
void GameClient::launchPve(int map, const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    loadMap(map);
    int mapIndex = _settings.getFactionSetting().getMapIndex(_settings.getFactionSetting().getThisFactionIndex());
    _settings.getFactionSetting().setContentSetting(setting, mapIndex);
    
    _mode = kPve;
    _looper.init(_settings, nullptr);
    
    _looper.start();
    
    _state = kPlaying;
}
    
void GameClient::launchPve(const MapSetting& mapSetting, const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    loadMap(mapSetting);
    int mapIndex = _settings.getFactionSetting().getMapIndex(_settings.getFactionSetting().getThisFactionIndex());
    _settings.getFactionSetting().setContentSetting(setting, mapIndex);
    
    _mode = kPve;
    _looper.init(_settings, nullptr);
    
    _looper.start();
    
    _state = kPlaying;
}

    
void GameClient::cancelLaunch() {
    if (_state != kLaunching) return;
    
    NetworkMessageCancel2S* msg = new NetworkMessageCancel2S();
    _proxy->send(msg);
}
    
void GameClient::quit() {
    if (_state != kPlaying) return;
    
    _looper.end();
    _state = kIdle;;
}
    
void GameClient::updateLaunching() {
    if (_state != kLaunching) return;
    
    NetworkMessage* msg = nullptr;
    for (msg = _proxy->pop(); msg; msg = _proxy->pop()) {
        if (dynamic_cast<NetworkMessageLaunch2C*>(msg)) {
            NetworkMessageLaunch2C* l2c = dynamic_cast<NetworkMessageLaunch2C*>(msg);
            loadMap(l2c->getMapId());
            _settings.setFactionSetting(l2c->getFactionSetting());
            _looper.init(_settings, &_net);
            _looper.start();
            _state = kPlaying;
        } else if (dynamic_cast<NetworkMessageCancel2C*>(msg)) {
            _proxy->disconnect();
            _state = kIdle;
        }
        delete msg;
        if (_state != kLaunching) {
            _schduler->unSchedule(SCHEDULE_KEY_CLIENT_LAUNCHING);
            break;
        }
    }
}
    
void GameClient::loadTechTree() {
    _settings.setTechTree(DataManager::getInstance()->getTechTreeData(1));
}

void GameClient::loadMap(int mapId) {
    _settings.setMapId(mapId);
    MapSetting ms;
    ms.init(DataManager::getInstance()->getMapData(mapId));
    loadMap(ms);
}
    
void GameClient::loadMap(const MapSetting& mapSetting) {
    _settings.setMapSetting(mapSetting);
    _settings.setFactionSetting(mapSetting.getFactionSetting());
}





}}