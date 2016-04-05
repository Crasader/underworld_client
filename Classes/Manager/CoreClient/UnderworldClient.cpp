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
    
UnderworldClient::UnderworldClient(const std::string& name,
    UnderWorld::Core::AbstractNetworkProxy* proxy,
    UnderWorld::Core::AbstractScheduler*  scheduler,
    UnderWorld::Core::AbstractRender* render)
: _mode(GAME_MODE_COUNT)
, _state(kIdle)
, _proxy(proxy)
, _scheduler(scheduler)
, _render(render)
, _name(name) {

}
    
void UnderworldClient::launchPvp(const GameContentSetting& setting) {
    if (_state != kIdle) return;
    
    loadTechTree();
    
    _mode = kPvp;
    _state = kLaunching;
    
//    //test code
//    {
//        loadMap(1);
//        FactionSetting fs;
//        fs.setThisFactionIndex(0);
//        fs.setFactionCount(2);
//        fs.setTeam(0, 0);
//        fs.setTeam(1, 1);
//        fs.setMapIndex(0, 0);
//        fs.setMapIndex(1, 1);
//        fs.setFactionControlType(0, kFactionControlType_Human);
//        fs.setFactionControlType(1, kFactionControlType_Human);
//        GameContentSetting gcs0;
//        GameContentSetting gcs1;
//        gcs0.setFactionTypeKey("狼人族");
//        gcs1.setFactionTypeKey("狼人族");
//        UnitSetting core;
//        core.setUnitTypeName("狼人基地");
//        gcs0.setCore(core);
//        gcs1.setCore(core);
//        UnitSetting tower;
//        tower.setUnitTypeName("狼人箭塔");
//        gcs0.setTower(tower);
//        gcs1.setTower(tower);
//        std::vector<std::string> cards0 = {
//            "巨人", "狼人步兵", "王子", "迷你皮卡", "高攻击战士"
//        };
//        vector<CardSetting> cs0;
//        for (int i = 0; i < cards0.size(); ++i) {
//            CardSetting cs;
//            cs.setCardTypeName(cards0[i]);
//            cs0.push_back(cs);
//        }
//        gcs0.setCards(cs0);
//        std::vector<std::string> cards1 = {
//            "巨人","战士","法师","火枪手","王子","迷你皮卡","骑士","高攻击战士"
//        };
//        vector<CardSetting> cs1;
//        for (int i = 0; i < cards1.size(); ++i) {
//            CardSetting cs;
//            cs.setCardTypeName(cards1[i]);
//            cs1.push_back(cs);
//        }
//        gcs1.setCards(cs1);
//        fs.setContentSetting(gcs0, 0);
//        fs.setContentSetting(gcs1, 1);
//        _settings.setFactionSetting(fs);
//        
//        _terminal.getGame().init(_settings);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(2, 0, Coordinate(1361, 584)), 138);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(2, 1, Coordinate(3275, 641)), 732);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(1, 0, Coordinate(1547, 667)), 786);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(2, 1, Coordinate(3200, 572)), 1320);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(1, 0, Coordinate(1599, 627)), 1410);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(2, 0, Coordinate(1599, 592)), 1968);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(3, 1, Coordinate(3323, 582)), 1992);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(0, 1, Coordinate(3200, 572)), 2328);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(2, 1, Coordinate(3296, 453)), 2640);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(1, 1, Coordinate(3784, 401)), 3162);
//        _terminal.getGame().getCommander()->addCommandFromRemote(new OutsideDeckCommand(3, 0, Coordinate(1599, 699)), 3168);
//        for (int i = 1; i <= 10000; ++i) {
//            _terminal.getGame().update();
//            if (_terminal.getGame().isGameOver()) {
//                cout << "[gameover]" << i << endl;
//                break;
//            }
//            
//            if (i >= 1000 && i <= 3300) {
//                std::string s;
//                _terminal.getGame().getSnapShot(s);
//                cout << "[update]" << i << s << endl;
//            }
//        }
//
//    }
    
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
    _terminal.init(_name, _settings, _scheduler, _render, nullptr);
    
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
    _terminal.init(_name, _settings, _scheduler, _render, nullptr);
    
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
            _terminal.init(_name, _settings, _scheduler, _render, _proxy);
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
    
    //TODO:temp code
    ms.setHeight(75);
    ms.setWidth(300);
    
    loadMap(ms);
}
    
void UnderworldClient::loadMap(const MapSetting& mapSetting) {
    _settings.setMapSetting(mapSetting);
    _settings.setFactionSetting(mapSetting.getFactionSetting());
}
