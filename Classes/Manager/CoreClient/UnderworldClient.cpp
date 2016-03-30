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
    
UnderworldClient::UnderworldClient(
    UnderWorld::Core::AbstractNetworkProxy* proxy,
    UnderWorld::Core::AbstractScheduler*  scheduler,
    UnderWorld::Core::AbstractRender* render)
: _mode(GAME_MODE_COUNT)
, _state(kIdle)
, _proxy(proxy)
, _scheduler(scheduler)
, _render(render) {

}
    
void UnderworldClient::launchPvp(const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    
    _mode = kPvp;
    _state = kLaunching;
    
    _proxy->connect();
    _proxy->registerListener(this);
    NetworkMessageLaunch2S* msg = new NetworkMessageLaunch2S();
    msg->setGameContentSetting(setting);
    _proxy->send(msg);
    
}
    
void UnderworldClient::launchPve(int map, const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    loadMap(map);
    int mapIndex = _settings.getFactionSetting().getMapIndex(_settings.getFactionSetting().getThisFactionIndex());
    _settings.getFactionSetting().setContentSetting(setting, mapIndex);
    
    _mode = kPve;
    _terminal.init(_settings, _scheduler, _render, nullptr);
    
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
    _terminal.init(_settings, _scheduler, _render, nullptr);
    
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

void UnderworldClient::onReceive(
    const std::vector<UnderWorld::Core::NetworkMessage*>& msgs) {
    if (_state != kLaunching) return;
    for (int i = 0; i < msgs.size(); ++i) {
        NetworkMessage* msg = msgs[i];
        if (dynamic_cast<NetworkMessageLaunch2C*>(msg)) {
            NetworkMessageLaunch2C* l2c = dynamic_cast<NetworkMessageLaunch2C*>(msg);
            loadMap(l2c->getMapId());
            _settings.setFactionSetting(l2c->getFactionSetting());
            _terminal.init(_settings, _scheduler, _render, _proxy);
            _terminal.start();
            _state = kPlaying;
        } else if (dynamic_cast<NetworkMessageCancel2C*>(msg)) {
            _state = kIdle;
        }
        
        if (_state != kLaunching) {
            _proxy->unregisterListener(this);
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
