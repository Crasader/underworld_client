//
//  BattleScene.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleScene.h"
#include "CocosUtils.h"
#include "GameSettings.h"
#include "DataManager.h"
#include "SoundManager.h"
#include "TechTree.h"
#include "GameData.h"
#include "FormationData.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif
#include "GameData.h"

BattleScene* BattleScene::create(int mapId)
{
    BattleScene *ret = new (nothrow) BattleScene();
    if (ret && ret->init(mapId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleScene::BattleScene()
:_mapId(0)
,_render(nullptr)
,_client(nullptr)
,_sch(nullptr)
, _proxy(nullptr) {}

BattleScene::~BattleScene()
{
    clear();
    CocosUtils::cleanMemory();
}

// on "init" you need to initialize your instance
bool BattleScene::init(int mapId)
{
    if (Scene::init() )
    {
        _mapId = mapId;
        return true;
    }
    
    return false;
}

void BattleScene::onEnter()
{
    Scene::onEnter();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(true);
#endif
    start();
}

void BattleScene::onExit()
{
    SoundManager::getInstance()->stopBackgroundMusic();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(false);
#endif
    Scene::onExit();
}

#pragma mark - GameRenderObserver
void BattleScene::onGameRenderRestart()
{
    start();
}

void BattleScene::start()
{
    if (_render) {
        clear();
    }
    
    // 1. render
    _render = new (nothrow) GameRender(this, "Vampire");
    _render->registerObserver(this);
    
    // 2. scheduler
    _sch = new (nothrow) GameScheduler();
    
//    static const pair<string, int> host = {"192.168.31.100", 9999};
    static const pair<string, int> host = {"123.57.221.242", 8888};
    _proxy = new (nothrow) ClientTCPNetworkProxy(host.first, host.second, "demoplayer", GameData::getInstance()->getUUID());

    // 3. game setting
    UnderWorld::Core::GameContentSetting contentSetting;
    
    contentSetting.setFactionTypeKey("狼人族");
    
    UnderWorld::Core::UnitSetting core;
    core.setUnitTypeName("狼人基地");
    contentSetting.setCore(core);
    
    UnderWorld::Core::UnitSetting tower;
    tower.setUnitTypeName("狼人箭塔");
    contentSetting.setTower(tower);
    
    // 4. start game
    _client = new (nothrow) UnderworldClient("mofish", _proxy, _sch, _render);
    
    vector<UnderWorld::Core::UnitSetting> unitPool;
    {
        auto dm = DataManager::getInstance();
        const auto& cards = dm->getCardDecks();
        
        for (auto iter = begin(cards); iter != end(cards); ++iter) {
            auto cardType = dm->getTechTree()->findUnitTypeByName(*iter);
            if (cardType) {
                UnderWorld::Core::UnitSetting us;
                us.setUnitTypeName(cardType->getName());
                us.setLevel(0);
                us.setQuality(0);
                us.setTalentLevel(0);
                unitPool.push_back(us);
            }
        }
    }
    
    auto formationData = GameData::getInstance()->currentUser()->getDefaultFormationData();
    if (formationData) {
        const auto& cards = formationData->getSpells();
        const auto& heroes = formationData->getHeroes();
        GameModeHMMSetting::InitUnitList unitList;
        {
            for (auto iter = begin(heroes); iter != end(heroes); ++iter) {
                static const Point basePosition(240, 120);
                const auto& coordinate = iter->first;
                
                pair<string, int> pair;
                pair.first = iter->second;
                pair.second = coordinate.y * FORMATION_WIDTH + coordinate.x;
                
                unitList.push_back(pair);
            }
        }
        _client->launchPve(_mapId, contentSetting, cards, unitList, unitPool);
//        _client->launchPvp(contentSetting, cards);
    } else {
        const vector<string> cards;
        GameModeHMMSetting::InitUnitList unitList;
        _client->launchPve(_mapId, contentSetting, cards, unitList, unitPool);
    }
}

void BattleScene::clear()
{
    if (_client) {
        _client->quit();
    }
    CC_SAFE_DELETE(_client);
    CC_SAFE_DELETE(_sch);
    CC_SAFE_DELETE(_render);
    CC_SAFE_DELETE(_proxy);
    removeAllChildren();
}
