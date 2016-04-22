//
//  UnderworldClient.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/3/12.
//
//

#include "cocos2d.h"

#include "UnderworldClient.h"
#include "DataManager.h"
#include "CoreUtils.h"
#include "GameRender.h"
#include "MapLayer.h"

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

UnderworldClient::~UnderworldClient() {
    if (_proxy) {
        _proxy->unregisterListener(this);
    }
}
    
void UnderworldClient::launchPvp(const GameContentSetting& setting,
    const std::vector<std::string>& cards) {
    if (_state != kIdle || !_proxy) return;
    
    loadTechTree();
    
    _mode = kPvp;
    _state = kLaunching;
    
    //test code
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
//            "巨人","狼人步兵","王子","迷你皮卡","高攻击战士"
//        };
//        vector<CardSetting> cs0;
//        for (int i = 0; i < cards0.size(); ++i) {
//            CardSetting cs;
//            cs.setCardTypeName(cards0[i]);
//            cs0.push_back(cs);
//        }
//        gcs0.setCards(cs0);
//        std::vector<std::string> cards1 = {
//            "火枪手","狗"
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
//        std::vector<std::string> vec = {
//            "594-0-2-1-3200_432",
//            "804-0-3-0-1575_845",
//            "1026-0-2-1-3200_649",
//            "1380-0-3-0-1555_711",
//            "1638-0-2-1-3200_534",
//            "1956-0-2-0-1599_732",
//            "2016-0-1-1-3356_508",
//            "2328-0-0-1-3709_355",
//            "2616-0-3-0-1599_639",
//            "2754-0-0-1-3574_673",
//            "3252-0-0-1-3543_628",
//            "3330-0-3-0-1585_580",
//            "3576-0-0-1-3425_512",
//            "3900-0-1-0-1599_529",
//            "4110-0-0-1-3537_465",
//            "4278-0-2-0-1599_660",
//            "4356-0-0-1-3417_443",
//            "4974-0-3-0-1465_614",
//            "4992-0-0-1-3350_422",
//            "5604-0-3-0-1599_627",
//            "6000-0-0-1-3594_425",
//            "6282-0-0-1-3615_598",
//            "6744-0-0-1-3463_534",
//            "7356-0-0-1-3513_558",
//            "7590-0-2-0-1400_615",
//            "7920-0-5-0-1523_547",
//            "7944-0-0-1-3415_445",
//            "8382-0-4-0-1599_889",
//            "8730-0-4-0-1599_911",
//            "9192-0-0-1-4516_465",
//            "9396-0-0-1-4455_421",
//            "9444-0-1-0-1599_492"
//        };
//        for (int i = 0; i< vec.size(); ++i) {
//            std::vector<std::string> split;
//            Utils::split(split, vec[i], "-");
//            int frame = std::atoi(split[0].c_str());
//            int handindex = std::atoi(split[2].c_str());
//            int factionindex = std::atoi(split[3].c_str());
//            int x = atoi(split[4].substr(0, split[4].find("_")).c_str());
//            int y = atoi(split[4].substr(split[4].find("_") + 1).c_str());
//            OutsideDeckCommand* cmd = new OutsideDeckCommand(handindex, factionindex, Coordinate32(x, y));
//            _terminal.getGame().getCommander()->addCommandFromRemote(cmd, frame);
//        }
//        for (int i = 1; i <= 24000; ++i) {
//            _terminal.getGame().update();
//            if (_terminal.getGame().isGameOver()) {
//                cout << "[gameover]" << i << endl;
//                break;
//            }
//            
//            if (i == 24000) {
//                std::string s;
//                _terminal.getGame().getSnapShot(s);
//                cout << "[update]" << i << s << endl;
//            }
//        }
//
//    }
    _proxy->registerListener(this);
    _proxy->connect();
    NetworkMessageLaunch2S* msg = new NetworkMessageLaunch2S();
    msg->setGameContentSetting(setting);
    msg->setCards(cards);
    _proxy->send(msg);
    
}
    
void UnderworldClient::launchPve(int map, const GameContentSetting& setting,
    const std::vector<std::string>& cards) {
    if (_state != kIdle) return;
    
    loadTechTree();
    loadMap(map);
    int mapIndex = _settings.getFactionSetting().getMapIndex(_settings.getFactionSetting().getThisFactionIndex());
    _settings.getFactionSetting().setContentSetting(setting, mapIndex);
    _hmmSetting._cards[mapIndex] = cards;
    
    _mode = kPve;
    _terminal.init(_name, _settings, new GameModeHMM(), &_hmmSetting,
        _scheduler, _render, nullptr);
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
            for (int i = 0; i < l2c->getCards().size(); ++i) {
                _hmmSetting._cards[i] = l2c->getCards()[i];
            }
            _terminal.init(_name, _settings, new GameModeHMM(),
                &_hmmSetting, _scheduler, _render, _proxy);
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
    _settings.setTechTree(DataManager::getInstance()->getTechTreeData());
    _hmmSetting._hmmTechTree = DataManager::getInstance()->getHMMTechTreeData();
}

void UnderworldClient::loadMap(int mapId) {
    _settings.setMapId(mapId);
    
    MapSetting ms;
    ms.setMapId(mapId);
    const cocos2d::experimental::TMXTiledMap* tiledMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("map/%d.tmx", mapId));
    cocos2d::experimental::TMXLayer *logicLayer = tiledMap->getLayer("logic");
    logicLayer->setVisible(false);
    const Size &logicSize = logicLayer->getLayerSize();
    ms.setWidth(logicSize.width);
    ms.setHeight(logicSize.height);
    for (unsigned int x = 0; x < logicSize.width; x++)
    {
        for (unsigned int y = 0; y < logicSize.height; y++)
        {
            int gid = logicLayer->getTileGIDAt(Vec2(x, y));
            if (gid == 0) {
                //can walk
            } else {
                //can not walk
                //TODO: land can't walk & air can't walk
                ms.addUnWalkableArea(UnderWorld::Core::Coordinate32(x, (logicSize.height - 1) - y), UnderWorld::Core::kFieldType_Land);
            }
        }
    }
    ms.init(DataManager::getInstance()->getMapData(mapId));
    
    _settings.setMapSetting(ms);
    _settings.setFactionSetting(ms.getFactionSetting());
    
    _hmmSetting.init(DataManager::getInstance()->getMapDataHMM(mapId));
}
