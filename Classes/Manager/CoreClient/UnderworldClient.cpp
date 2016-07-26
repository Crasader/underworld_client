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
    const std::vector<std::string>& cards,
    const UnderWorld::Core::GameModeHMMSetting::InitUnitList& initUnits,
    const std::vector<UnitSetting>& unitPool) {
    if (_state != kIdle || !_proxy) return;
    
    loadTechTree();
    
    _mode = kPvp;
    _state = kLaunching;
    
    //test code
//    {
//        loadMap(4);
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
//        
//        fs.setContentSetting(gcs0, 0);
//        fs.setContentSetting(gcs1, 1);
//        _settings.setFactionSetting(fs);
//        
//        std::vector<std::string> cards0 = {
//            "酸雾术lv1","震晕法术lv1","魔族梦魇精华","魔族骷髅王子","暴风雪lv1","医疗术lv1"
//        };
//        std::vector<std::string> cards1 = {
//            "梦魇","石像鬼","食尸鬼","骷髅弓箭手","骷髅盾牌兵","魔族传教士","暴风雪lv1","震晕法术lv1"
//        };
//        
//        _hmmSetting._cards[0] = cards0;
//        _hmmSetting._cards[1] = cards1;
//    
//        
//        _terminal.getGame().init(_settings, new GameModeHMM(), &_hmmSetting);
//        std::vector<std::string> vec = {
//            "234-0-3-0-246_363",
//            "390-0-1-1-818_309",
//            "480-0-2-1-733_343",
//            "732-0-2-1-747_282",
//            "1026-0-5-0-718_334",
//            "1116-0-2-1-786_327",
//            "2778-0-1-1-804_285",
//            "3090-0-6-1-845_261",
//            "3138-0-5-0-807_240",
//            "3318-0-7-1-892_245",
//            "3678-0-4-0-741_324",
//            "3708-0-0-1-968_196",
//            "3792-0-1-1-848_232",
//            "3882-0-1-1-1000_326",
//            "4332-0-2-0-316_330",
//            "4956-0-0-1-734_208",
//            "5772-0-4-0-274_301",
//            "5916-0-3-0-151_226",
//            "6510-0-4-0-251_313",
//            "6648-0-4-0-303_313"
//        };
//        for (int i = 0; i< vec.size(); ++i) {
//            std::vector<std::string> split;
//            UnderWorldCoreUtils::split(split, vec[i], "-");
//            int frame = std::atoi(split[0].c_str());
//            int handindex = std::atoi(split[2].c_str());
//            int factionindex = std::atoi(split[3].c_str());
//            int x = atoi(split[4].substr(0, split[4].find("_")).c_str());
//            int y = atoi(split[4].substr(split[4].find("_") + 1).c_str());
//            OutsideHMMCommand* cmd = new OutsideHMMCommand(handindex, factionindex, Coordinate32(x, y));
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
//    }
    _proxy->registerListener(this);
    _proxy->connect();
    NetworkMessageLaunch2S* msg = new NetworkMessageLaunch2S();
    msg->setGameContentSetting(setting);
    msg->setCards(cards);
    msg->setInitUnits(initUnits);
    msg->setUnitPool(unitPool);
    _proxy->send(msg);
    
}
    
void UnderworldClient::launchPve(int map, const GameContentSetting& setting,
    const std::vector<std::string>& cards,
    const UnderWorld::Core::GameModeHMMSetting::InitUnitList& initList,
    const std::vector<UnderWorld::Core::UnitSetting>& unitPool) {
    if (_state != kIdle) return;
    
    loadTechTree();
    loadMap(map);
    int mapIndex = _settings.getFactionSetting().getMapIndex(_settings.getFactionSetting().getThisFactionIndex());
    _settings.getFactionSetting().setContentSetting(setting, mapIndex);
    _hmmSetting._cards[mapIndex] = cards;
    _hmmSetting._initUnits[mapIndex] = initList;
    _hmmSetting._unitPools[mapIndex] = unitPool;
    
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
            for (int i = 0; i < l2c->getInitUnitLists().size(); ++i) {
                _hmmSetting._initUnits[i] = l2c->getInitUnitLists()[i];
            }
            for (int i = 0; i < l2c->getUnitPools().size(); ++i) {
                _hmmSetting._unitPools[i] = l2c->getUnitPools()[i];
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
    std::string commonTechTree;
    loadCommonTechTree(commonTechTree);
    _settings.setTechTree(commonTechTree);
    
    std::string hmmTechTree;
    loadHMMMTechTree(_hmmSetting._hmmTechTree);
}

void UnderworldClient::loadMap(int mapId) {
    _settings.setMapId(mapId);
    
    MapSetting ms;
    loadCommonMapSetting(mapId, ms);
    _settings.setMapSetting(ms);
    _settings.setFactionSetting(ms.getFactionSetting());
    
    std::string hmms;
    loadHMMMapSetting(mapId, hmms);
    _hmmSetting.init(hmms);
}

void UnderworldClient::loadCommonMapSetting(int mapId, MapSetting& output) {
    output.setMapId(mapId);
    const cocos2d::experimental::TMXTiledMap* tiledMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("map/%d.tmx", mapId));
    cocos2d::experimental::TMXLayer *logicLayer = tiledMap->getLayer("logic");
    logicLayer->setVisible(false);
    const Size &logicSize = logicLayer->getLayerSize();
    output.setWidth(logicSize.width);
    output.setHeight(logicSize.height);
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
                output.addUnWalkableArea(UnderWorld::Core::Coordinate32(x, (logicSize.height - 1) - y), UnderWorld::Core::kFieldType_Land);
            }
        }
    }
    output.init(DataManager::getInstance()->getMapData(mapId));
}

void UnderworldClient::loadHMMMapSetting(int mapId, std::string& output) {
    output = DataManager::getInstance()->getMapDataHMM(mapId);
}

void UnderworldClient::loadCommonTechTree(std::string &output) {
    output = DataManager::getInstance()->getTechTreeData();
}

void UnderworldClient::loadHMMMTechTree(std::string &output) {
    output = DataManager::getInstance()->getHMMTechTreeData();
}



