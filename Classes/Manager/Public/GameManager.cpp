//
//  GameManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/18.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GameManager.h"
#include "GameScheduler.h"
#include "ClientTCPNetwork.h"
#include "GameSettings.h"
#include "GameModeHMM.h"
#include "TechTree.h"
#include "GameData.h"
#include "DataManager.h"
#include "FrameLoader.h"
#include "DeckManager.h"
#include "DeckData.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "BattleScene.h"
#include "MainScene.h"
#include "MessageBoxLayer.h"
#include "UWRender.h"
#include "GameLooper.h"

using namespace std;
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
        std::vector<std::string> cardStrVec;
        UnderWorld::Core::UnderWorldCoreUtils::split(cardStrVec, cardStr, "|");
        
        for (int i = 0; i < cardStrVec.size(); ++i) {
            hmm._cards[index].push_back(atoi(cardStrVec[i].c_str()));
        }
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
                pair<int, int> pair;
                pair.first = atoi(pairVec[0].c_str());
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
                us.setUnitTypeId(atoi(vec[0].c_str()));
                us.setLevel(atoi(vec[1].c_str()));
                us.setQuality(atoi(vec[2].c_str()));
                us.setTalentLevel(atoi(vec[3].c_str()));
                hmm._unitPools[index].push_back(us);
            }
        }
    }
}

struct GameClient::BattleContent {
    UnderWorld::Core::GameContentSetting contentSetting;
    vector<int> cards;
    GameModeHMMSetting::InitUnitList unitList;
    vector<UnderWorld::Core::UnitSetting> unitPool;
};

GameClient::GameClient()
: _mapId(0)
, _mode(GAME_MODE_COUNT)
, _state(kIdle)
, _scene(nullptr)
, _render(nullptr)
, _gameScheduler(nullptr)
, _network(nullptr)
, _looper(nullptr)
, _battleContent(nullptr)
, _settings(nullptr)
, _hmmSettings(nullptr) {
}

GameClient::~GameClient()
{
    purgeInstance();
    CC_SAFE_DELETE(_battleContent);
}

#pragma mark - public
void GameClient::launchPve(int mapId)
{
    if (_state != kIdle) return;
    
    resetStatus();
    _mode = kPve;
    _mapId = mapId;
    launchGame();
}

void GameClient::launchPvp()
{
    if (_state != kIdle) return;
    
    _mode = kPvp;
    launchGame();
}

void GameClient::cancelLaunch() {
    if (_state != kLaunchingNetwork) return;
    
    exitGame();
}

void GameClient::launchGame()
{
    if (_state != kIdle) return;
    
    if (loadBattleContent()) {
        _state = kLaunchingPVR;
        createInstance();
        onPVRLoaded();
    } else {
        MessageBoxLayer::getInstance()->show(LocalHelper::getString("hint_selectNoCard"));
    }
}

void GameClient::exitGame()
{
    if (_mode == kPvp && _state == kPlaying && _network) {
        _network->closeGame();
    }
    
    purgeInstance();
    FrameLoader::getInstance()->removeCachedFrames();
    CocosUtils::cleanMemory();
    _state = kIdle;
}

void GameClient::restartPve() {
}

void GameClient::createInstance()
{
    purgeInstance();
    
    // 1. scene
    _scene = BattleScene::create();
    _scene->retain();

    // 2. render
    _render = new (nothrow) UWRender(_scene);
    _render->setRenderListener(this);
    
    // 3. scheduler
    _gameScheduler = new (nothrow) GameScheduler();
    
    // 4. network
//    static const pair<string, int> host = {"192.168.31.139", 9999};
    static const pair<string, int> host = {"123.57.221.242", 9999};
    _network = new (nothrow) ClientTCPNetwork(host.first, host.second, "mofish", GameData::getInstance()->getUUID());
    
    // 5. looper
    _looper = new (nothrow) UnderWorld::Core::GameLooper();
    
    // 6. settings
    _settings = new UnderWorld::Core::GameSettings();
    _hmmSettings = new UnderWorld::Core::GameModeHMMSetting();
}

void GameClient::purgeInstance()
{
    CC_SAFE_DELETE(_settings);
    CC_SAFE_DELETE(_hmmSettings);
    CC_SAFE_DELETE(_render);
    CC_SAFE_DELETE(_looper);
    CC_SAFE_DELETE(_network);
    CC_SAFE_DELETE(_gameScheduler);
    CC_SAFE_RELEASE_NULL(_scene);
}

bool GameClient::loadBattleContent()
{
    if (!_battleContent) {
        _battleContent = new BattleContent();
        
        {
            auto& cs = _battleContent->contentSetting;
            cs.setFactionTypeKey("狼人族");
            
            UnderWorld::Core::UnitSetting core;
            core.setUnitTypeId(10000);
            cs.setCore(core);
        }
        
        {
            auto dm = DataManager::getInstance();
            const auto& cards = dm->getCardDecks();
            
            for (auto iter = begin(cards); iter != end(cards); ++iter) {
                auto cardType = dm->getTechTree()->findUnitTypeById(*iter);
                if (cardType) {
                    UnderWorld::Core::UnitSetting us;
                    us.setUnitTypeId(cardType->getId());
                    //us.setUnitTypeName(cardType->getName());
                    us.setLevel(0);
                    us.setQuality(0);
                    us.setTalentLevel(0);
                    _battleContent->unitPool.push_back(us);
                }
            }
        }
    }
    
    // reload cards data every time
    auto data = DeckManager::getInstance()->getThisDeckData();
    
    _battleContent->cards.clear();
    _battleContent->unitList.clear();
    
    if (data) {
        for (auto card : data->getCards()) {
            _battleContent->cards.push_back(card);
        }
    }
    
    return !_battleContent->cards.empty();
}

void GameClient::resetStatus() {
    _mapId = 0;
    _mode = GAME_MODE_COUNT;
    _state = kIdle;
}

void GameClient::onPVRLoaded()
{
    if (_state != kLaunchingPVR) return;
    
    CocosUtils::replaceScene(_scene, true);
    
    do {
        if (!_settings || !_hmmSettings || !_looper || !_battleContent) {
            _state = kIdle;
            break;
        }
        
        if (_mode == kPve) {
            loadTechTree2Settings();
            loadMap2Settings(_mapId);
            int mapIndex = _settings->getFactionSetting().getMapIndex(_settings->getFactionSetting().getThisFactionIndex());
            _settings->getFactionSetting().setContentSetting(_battleContent->contentSetting, mapIndex);
            _hmmSettings->_cards[mapIndex] = _battleContent->cards;
            _hmmSettings->_initUnits[mapIndex] = _battleContent->unitList;
            _hmmSettings->_unitPools[mapIndex] = _battleContent->unitPool;
            
            _looper->init("mofish",
                          *_settings,
                          new GameModeHMM(),
                          _hmmSettings,
                          _gameScheduler,
                          _render,
                          nullptr);
            _looper->start();
            _state = kPlaying;
        } else if (_mode == kPvp) {
            loadTechTree2Settings();
            
//            test code
//            {
//                loadMap(3);
//                FactionSetting fs;
//                fs.setThisFactionIndex(0);
//                fs.setFactionCount(2);
//                fs.setTeam(0, 0);
//                fs.setTeam(1, 1);
//                fs.setMapIndex(0, 0);
//                fs.setMapIndex(1, 1);
//                fs.setFactionControlType(0, kFactionControlType_Human);
//                fs.setFactionControlType(1, kFactionControlType_Human);
//                GameContentSetting gcs0;
//                GameContentSetting gcs1;
//                gcs0.setFactionTypeKey("狼人族");
//                gcs1.setFactionTypeKey("狼人族");
//                UnitSetting core;
//                core.setUnitTypeName("狼人基地");
//                gcs0.setCore(core);
//                gcs1.setCore(core);
//                UnitSetting tower;
//                tower.setUnitTypeName("狼人箭塔");
//                gcs0.setTower(tower);
//                gcs1.setTower(tower);
//        
//                fs.setContentSetting(gcs0, 0);
//                fs.setContentSetting(gcs1, 1);
//                _settings.setFactionSetting(fs);
//        
//        //        std::vector<std::string> cards0 = {
//        //            "酸雾术lv1","震晕法术lv1","魔族梦魇精华","魔族骷髅王子","暴风雪lv1","医疗术lv1"
//        //        };
//        //        std::vector<std::string> cards1 = {
//        //            "梦魇","石像鬼","食尸鬼","骷髅弓箭手","骷髅盾牌兵","魔族传教士","暴风雪lv1","震晕法术lv1"
//        //        };
//        //
//        //        _hmmSetting._cards[0] = cards0;
//        //        _hmmSetting._cards[1] = cards1;
//        
//                parseUserInfo("小型驱散术lv1|人族弓手|人族女巫|人族狮鹫|骷髅盾牌兵|暴风雪lv1|魔族黑传教士|魔族梦魇精华|梦魇|石像鬼--中立兽族巨魔巫医|0|0|0,中立兽族斧王|0|0|0,中立兽族牛头人|0|0|0,中立兽族食人鬼王|0|0|0,中立矮人族火枪手|0|0|0,中立精灵族弓箭手|0|0|0,中立龙族黑龙|0|0|0,人族剑士|0|0|0,人族剑士队长|0|0|0,人族大法师|0|0|0,人族女巫|0|0|0,人族弓手|0|0|0,人族弓箭队长|0|0|0,人族狮鹫|0|0|0,人族男巫|0|0|0,人族盾牌兵|0|0|0,人族盾牌队长|0|0|0,人族秘法大师|0|0|0,梦魇|0|0|0,石像鬼|0|0|0,食尸鬼|0|0|0,骷髅弓箭手|0|0|0,骷髅盾牌兵|0|0|0,魔族传教士|0|0|0,魔族地狱战舰|0|0|0,魔族梦魇精华|0|0|0,魔族食尸鬼王|0|0|0,魔族骷髅王子|0|0|0,魔族黑传教士|0|0|0", _hmmSetting, 0);
//                parseUserInfo("梦魇|魔族传教士|骷髅盾牌兵|人族弓手|人族剑士|小型驱散术lv1|暴风雪lv1|医疗术lv1|人族盾牌队长|魔族黑传教士--中立兽族巨魔巫医|0|0|0,中立兽族斧王|0|0|0,中立兽族牛头人|0|0|0,中立兽族食人鬼王|0|0|0,中立矮人族火枪手|0|0|0,中立精灵族弓箭手|0|0|0,中立龙族黑龙|0|0|0,人族剑士|0|0|0,人族剑士队长|0|0|0,人族大法师|0|0|0,人族女巫|0|0|0,人族弓手|0|0|0,人族弓箭队长|0|0|0,人族狮鹫|0|0|0,人族男巫|0|0|0,人族盾牌兵|0|0|0,人族盾牌队长|0|0|0,人族秘法大师|0|0|0,梦魇|0|0|0,石像鬼|0|0|0,食尸鬼|0|0|0,骷髅弓箭手|0|0|0,骷髅盾牌兵|0|0|0,魔族传教士|0|0|0,魔族地狱战舰|0|0|0,魔族梦魇精华|0|0|0,魔族食尸鬼王|0|0|0,魔族骷髅王子|0|0|0,魔族黑传教士|0|0|0", _hmmSetting, 1);
//        
//        
//                _terminal.getGame().init(_settings, new GameModeHMM(), &_hmmSetting);
//                std::vector<std::string> vec = {
//                    "168-0-4-0-33_240",
//                    "168-0-4-0-33_240",
//                    "306-0-2-1-963_203",
//                    "306-0-2-1-963_203",
//                    "342-0-7-0-13_253",
//                    "342-0-7-0-13_253"
//                };
//                for (int i = 0; i< vec.size(); ++i) {
//                    std::vector<std::string> split;
//                    UnderWorldCoreUtils::split(split, vec[i], "-");
//                    int frame = std::atoi(split[0].c_str());
//                    int handindex = std::atoi(split[2].c_str());
//                    int factionindex = std::atoi(split[3].c_str());
//                    int x = atoi(split[4].substr(0, split[4].find("_")).c_str());
//                    int y = atoi(split[4].substr(split[4].find("_") + 1).c_str());
//                    OutsideHMMCommand* cmd = new OutsideHMMCommand(handindex, factionindex, Coordinate32(x, y));
//                    _terminal.getGame().getCommander()->addCommandFromRemote(cmd, frame);
//                }
//                for (int i = 1; i <= 24000; ++i) {
//                    _terminal.getGame().update();
//                    if (_terminal.getGame().isGameOver()) {
//                        cout << "[gameover]" << i << endl;
//                        break;
//                    }
//                    
//                    if (i == 24000) {
//                        std::string s;
//                        _terminal.getGame().getSnapShot(s);
//                        cout << "[update]" << i << s << endl;
//                    }
//                }
//            }
            
            if (_network) {
                _network->connect();
                _network->launchGame(this,
                    _battleContent->contentSetting,
                    _battleContent->cards,
                    _battleContent->unitList,
                    _battleContent->unitPool);
                _state = kLaunchingNetwork;
            } else {
                _state = kIdle;
                break;
            }
            
        } else {
            _state = kIdle;
            break;
        }

    } while (0);
    
    if (_state == kIdle) exitGame();
    
}

void GameClient::loadTechTree2Settings() {
    std::string commonTechTree;
    loadCommonTechTree(commonTechTree);
    if (_settings) _settings->setTechTree(commonTechTree);

    if (_hmmSettings) loadHMMMTechTree(_hmmSettings->_hmmTechTree);
}

void GameClient::loadMap2Settings(int mapId) {
    if (_settings) {
        _settings->setMapId(mapId);
        UnderWorld::Core::MapSetting ms;
        loadCommonMapSetting(mapId, ms);
        _settings->setMapSetting(ms);
        _settings->setFactionSetting(ms.getFactionSetting());
    }
    
    if (_hmmSettings) {
        std::string hmms;
        loadHMMMapSetting(mapId, hmms);
        _hmmSettings->init(hmms);
    }
}

void GameClient::onLaunched(const NetworkMessageLaunch2C& l2c) {
    if (_state != kLaunchingNetwork) return;
    
    loadMap2Settings(l2c.getMapId());
    _settings->setFactionSetting(l2c.getFactionSetting());
    for (int i = 0; i < l2c.getCards().size(); ++i) {
        _hmmSettings->_cards[i] = l2c.getCards()[i];
    }
    for (int i = 0; i < l2c.getInitUnitLists().size(); ++i) {
        _hmmSettings->_initUnits[i] = l2c.getInitUnitLists()[i];
    }
    for (int i = 0; i < l2c.getUnitPools().size(); ++i) {
        _hmmSettings->_unitPools[i] = l2c.getUnitPools()[i];
    }
    
    _looper->init("mofish", *_settings, new GameModeHMM(),
        _hmmSettings, _gameScheduler, _render, _network);
    _looper->start();
    _state = kPlaying;
}

void GameClient::onExitRequest() {
    if (_state != kPlaying) return;
    
    exitGame();
    CocosUtils::replaceScene(MainScene::create(), true);
}

bool GameClient::onPauseRequest() {
    if (_state != kPlaying || !_looper) return false;
    return _looper->pause();
    
}
bool GameClient::onResumeRequest() {
    if (_state != kPlaying || !_looper) return false;
    
    _looper->start();
    return true;
}

void GameClient::loadCommonMapSetting(int mapId, UnderWorld::Core::MapSetting& output) {
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

void GameClient::loadHMMMapSetting(int mapId, std::string& output) {
    output = DataManager::getInstance()->getMapDataHMM(mapId);
}

void GameClient::loadCommonTechTree(std::string &output) {
    output = DataManager::getInstance()->getTechTreeData();
}

void GameClient::loadHMMMTechTree(std::string &output) {
    output = DataManager::getInstance()->getHMMTechTreeData();
}

static GameClient* s_pInstance(nullptr);
GameClient* GameManager::getGameClient()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) GameClient();
    }
    
    return s_pInstance;
}

void GameManager::purgeGameClient()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}



