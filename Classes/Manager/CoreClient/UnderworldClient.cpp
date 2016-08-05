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

static void parseUserInfo(const std::string& dataStr, UnderWorld::Core::GameModeHMMSetting& hmm, int index)
{
    std::vector<string> data;
    UnderWorld::Core::UnderWorldCoreUtils::split(data, dataStr, "-");
    if (data.size() != 3) {
        return;
    }
    
    //    std::vector<string> cards;
    //    cards.swap(_cards);
    
    string cardStr = data[0];
    string unitStr = data[1];
    string poolStr = data[2];
    if(!cardStr.empty()) {
        UnderWorld::Core::UnderWorldCoreUtils::split(hmm._cards[index], cardStr, "|");
    }
    
    if (!unitStr.empty()) {
        std::vector<string> initUnits;
        UnderWorld::Core::UnderWorldCoreUtils::split(initUnits, unitStr, ",");
        {
            for (auto iter = initUnits.begin(); iter != initUnits.end(); ++iter) {
                const string& initUnit = *iter;
                std::vector<string> pairVec;
                UnderWorld::Core::UnderWorldCoreUtils::split(pairVec, initUnit, "|");
                if (pairVec.size() != 2) {
                    continue;
                }
                pair<string, int> pair;
                pair.first = pairVec[0];
                pair.second = atoi(pairVec[1].c_str());
                hmm._initUnits[index].push_back(pair);
            }
        }
    }
    
    if(!poolStr.empty()) {
        std::vector<string> unitPools;
        UnderWorld::Core::UnderWorldCoreUtils::split(unitPools, poolStr, ",");
        {
            for (auto iter = begin(unitPools); iter != end(unitPools); ++iter) {
                std::vector<string> vec;
                UnderWorld::Core::UnderWorldCoreUtils::split(vec, *iter, "|");
                
                if (vec.size() != 4) {
                    continue;
                }
                UnderWorld::Core::UnitSetting us;
                us.setUnitTypeName(vec[0]);
                us.setLevel(atoi(vec[1].c_str()));
                us.setQuality(atoi(vec[2].c_str()));
                us.setTalentLevel(atoi(vec[3].c_str()));
                hmm._unitPools[index].push_back(us);
            }
        }
    }
}

    
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
    
//    test code
    {
        loadMap(3);
        FactionSetting fs;
        fs.setThisFactionIndex(0);
        fs.setFactionCount(2);
        fs.setTeam(0, 0);
        fs.setTeam(1, 1);
        fs.setMapIndex(0, 0);
        fs.setMapIndex(1, 1);
        fs.setFactionControlType(0, kFactionControlType_Human);
        fs.setFactionControlType(1, kFactionControlType_Human);
        GameContentSetting gcs0;
        GameContentSetting gcs1;
        gcs0.setFactionTypeKey("狼人族");
        gcs1.setFactionTypeKey("狼人族");
        UnitSetting core;
        core.setUnitTypeName("狼人基地");
        gcs0.setCore(core);
        gcs1.setCore(core);
        UnitSetting tower;
        tower.setUnitTypeName("狼人箭塔");
        gcs0.setTower(tower);
        gcs1.setTower(tower);
        
        fs.setContentSetting(gcs0, 0);
        fs.setContentSetting(gcs1, 1);
        _settings.setFactionSetting(fs);
        
//        std::vector<std::string> cards0 = {
//            "酸雾术lv1","震晕法术lv1","魔族梦魇精华","魔族骷髅王子","暴风雪lv1","医疗术lv1"
//        };
//        std::vector<std::string> cards1 = {
//            "梦魇","石像鬼","食尸鬼","骷髅弓箭手","骷髅盾牌兵","魔族传教士","暴风雪lv1","震晕法术lv1"
//        };
//        
//        _hmmSetting._cards[0] = cards0;
//        _hmmSetting._cards[1] = cards1;
        
        parseUserInfo("小型驱散术lv1|人族弓手|人族女巫|人族狮鹫|骷髅盾牌兵|暴风雪lv1|魔族黑传教士|魔族梦魇精华|梦魇|石像鬼--中立兽族巨魔巫医|0|0|0,中立兽族斧王|0|0|0,中立兽族牛头人|0|0|0,中立兽族食人鬼王|0|0|0,中立矮人族火枪手|0|0|0,中立精灵族弓箭手|0|0|0,中立龙族黑龙|0|0|0,人族剑士|0|0|0,人族剑士队长|0|0|0,人族大法师|0|0|0,人族女巫|0|0|0,人族弓手|0|0|0,人族弓箭队长|0|0|0,人族狮鹫|0|0|0,人族男巫|0|0|0,人族盾牌兵|0|0|0,人族盾牌队长|0|0|0,人族秘法大师|0|0|0,梦魇|0|0|0,石像鬼|0|0|0,食尸鬼|0|0|0,骷髅弓箭手|0|0|0,骷髅盾牌兵|0|0|0,魔族传教士|0|0|0,魔族地狱战舰|0|0|0,魔族梦魇精华|0|0|0,魔族食尸鬼王|0|0|0,魔族骷髅王子|0|0|0,魔族黑传教士|0|0|0", _hmmSetting, 0);
        parseUserInfo("梦魇|魔族传教士|骷髅盾牌兵|人族弓手|人族剑士|小型驱散术lv1|暴风雪lv1|医疗术lv1|人族盾牌队长|魔族黑传教士--中立兽族巨魔巫医|0|0|0,中立兽族斧王|0|0|0,中立兽族牛头人|0|0|0,中立兽族食人鬼王|0|0|0,中立矮人族火枪手|0|0|0,中立精灵族弓箭手|0|0|0,中立龙族黑龙|0|0|0,人族剑士|0|0|0,人族剑士队长|0|0|0,人族大法师|0|0|0,人族女巫|0|0|0,人族弓手|0|0|0,人族弓箭队长|0|0|0,人族狮鹫|0|0|0,人族男巫|0|0|0,人族盾牌兵|0|0|0,人族盾牌队长|0|0|0,人族秘法大师|0|0|0,梦魇|0|0|0,石像鬼|0|0|0,食尸鬼|0|0|0,骷髅弓箭手|0|0|0,骷髅盾牌兵|0|0|0,魔族传教士|0|0|0,魔族地狱战舰|0|0|0,魔族梦魇精华|0|0|0,魔族食尸鬼王|0|0|0,魔族骷髅王子|0|0|0,魔族黑传教士|0|0|0", _hmmSetting, 1);
    
        
        _terminal.getGame().init(_settings, new GameModeHMM(), &_hmmSetting);
        std::vector<std::string> vec = {
            "168-0-4-0-33_240",
            "168-0-4-0-33_240",
            "306-0-2-1-963_203",
            "306-0-2-1-963_203",
            "342-0-7-0-13_253",
            "342-0-7-0-13_253"
        };
        for (int i = 0; i< vec.size(); ++i) {
            std::vector<std::string> split;
            UnderWorldCoreUtils::split(split, vec[i], "-");
            int frame = std::atoi(split[0].c_str());
            int handindex = std::atoi(split[2].c_str());
            int factionindex = std::atoi(split[3].c_str());
            int x = atoi(split[4].substr(0, split[4].find("_")).c_str());
            int y = atoi(split[4].substr(split[4].find("_") + 1).c_str());
            OutsideHMMCommand* cmd = new OutsideHMMCommand(handindex, factionindex, Coordinate32(x, y));
            _terminal.getGame().getCommander()->addCommandFromRemote(cmd, frame);
        }
        for (int i = 1; i <= 24000; ++i) {
            _terminal.getGame().update();
            if (_terminal.getGame().isGameOver()) {
                cout << "[gameover]" << i << endl;
                break;
            }
            
            if (i == 24000) {
                std::string s;
                _terminal.getGame().getSnapShot(s);
                cout << "[update]" << i << s << endl;
            }
        }
    }
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


