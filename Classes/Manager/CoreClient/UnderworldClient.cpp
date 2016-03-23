//
//  UnderworldClient.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/3/12.
//
//

#include "UnderworldClient.h"
#include "DataManager.h"

using namespace UnderWorld::Core;

const std::string UnderworldClient::SCHEDULE_KEY_CLIENT_LAUNCHING("schedule_key_client_launching");
    
UnderworldClient::UnderworldClient(AbstractRender* render,
    AbstractScheduler* scheduler, AbstractNetworkProxy* proxy)
: _terminal(render, scheduler)
, _net(proxy)
, _mode(GAME_MODE_COUNT)
, _state(kIdle)
, _schduler(scheduler)
, _proxy(proxy) {
}
    
void UnderworldClient::launchPvp(const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    
    _mode = kPvp;
    
    _proxy->connect();
    NetworkMessageLaunch2S* msg =
    new NetworkMessageLaunch2S();
    msg->setGameContentSetting(setting);
    _proxy->send(msg);
    _schduler->schedule(CLIENT_LAUNCHING_FPS,
        std::bind(&UnderworldClient::updateLaunching, this),
        SCHEDULE_KEY_CLIENT_LAUNCHING);
    _state = kLaunching;
}
    
void UnderworldClient::launchPve(int map, const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    loadMap(map);
    int mapIndex = _settings.getFactionSetting().getMapIndex(_settings.getFactionSetting().getThisFactionIndex());
    _settings.getFactionSetting().setContentSetting(setting, mapIndex);
    
    _mode = kPve;
    _terminal.init(_settings, nullptr);
    
    _terminal.start();
    
    _state = kPlaying;
}
    
void UnderworldClient::launchPve(const MapSetting& mapSetting, const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    loadMap(mapSetting);
    int mapIndex = _settings.getFactionSetting().getMapIndex(_settings.getFactionSetting().getThisFactionIndex());
    _settings.getFactionSetting().setContentSetting(setting, mapIndex);
    
    _mode = kPve;
    _terminal.init(_settings, nullptr);
    
    _terminal.start();
    
    _state = kPlaying;
}

    
void UnderworldClient::cancelLaunch() {
    if (_state != kLaunching) return;
    
    NetworkMessageCancel2S* msg = new NetworkMessageCancel2S();
    _proxy->send(msg);
}
    
void UnderworldClient::quit() {
    if (_state != kPlaying) return;
    
    _terminal.end();
    _state = kIdle;;
}
    
void UnderworldClient::updateLaunching() {
    if (_state != kLaunching) return;
    
    NetworkMessage* msg = nullptr;
    for (msg = _proxy->pop(); msg; msg = _proxy->pop()) {
        if (dynamic_cast<NetworkMessageLaunch2C*>(msg)) {
            NetworkMessageLaunch2C* l2c = dynamic_cast<NetworkMessageLaunch2C*>(msg);
            loadMap(l2c->getMapId());
            _settings.setFactionSetting(l2c->getFactionSetting());
            _terminal.init(_settings, &_net);
            _terminal.start();
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
    
void UnderworldClient::loadTechTree() {
    _settings.setTechTree(DataManager::getInstance()->getTechTreeData(1));
}

void UnderworldClient::loadMap(int mapId) {
    _settings.setMapId(mapId);
    MapSetting ms;
    ms.init(DataManager::getInstance()->getMapData(mapId));
    loadMap(ms);
}
    
void UnderworldClient::loadMap(const MapSetting& mapSetting) {
    _settings.setMapSetting(mapSetting);
    _settings.setFactionSetting(mapSetting.getFactionSetting());
}
