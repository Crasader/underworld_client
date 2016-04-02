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
#include "UserDefaultsDataManager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif

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
{
    
}

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
    
    // 1. add map layer
    string mapSettingXml = DataManager::getInstance()->getMapData(_mapId);
    
    // 2. add map ui layer
    _render = new (nothrow) GameRender(this, _mapId, mapSettingXml, "Vampire");
    _render->registerObserver(this);
    
    _sch = new (nothrow) GameScheduler();
    
    // 3. game setting
    UnderWorld::Core::GameContentSetting contentSetting;
    
    contentSetting.setFactionTypeKey("狼人族");
    
    UnderWorld::Core::UnitSetting core;
    core.setUnitTypeName("狼人基地");
    contentSetting.setCore(core);
    
    UnderWorld::Core::UnitSetting tower;
    tower.setUnitTypeName("狼人箭塔");
    contentSetting.setTower(tower);
    
    set<string> cards;
    std::vector<UnderWorld::Core::CardSetting> cardSettings;
    UserDefaultsDataManager::getInstance()->getSelectedCards(cards);
    
    int i = 0;
    for (auto iter = begin(cards); iter != end(cards); ++iter, ++i) {
        UnderWorld::Core::CardSetting cs;
        cs.setCardTypeName(*iter);
        cardSettings.push_back(cs);
    }
    contentSetting.setCards(cardSettings);
    
    
    _client = new (nothrow) UnderworldClient("mofish", nullptr, _sch, _render);
    _client->launchPvp(contentSetting);
}

void BattleScene::startTest() {
    if (_render) {
        clear();
    }
    
    // 1. add map layer
    string mapSettingXml = DataManager::getInstance()->getMapData(_mapId);
    
    // 2. add map ui layer
    _render = new (nothrow) GameRender(this, _mapId, mapSettingXml, "Vampire");
    _render->registerObserver(this);
    
    // 3. game setting
    UnderWorld::Core::GameContentSetting contentSetting;
    
    contentSetting.setFactionTypeKey("狼人族");
    
    UnderWorld::Core::UnitSetting core;
    core.setUnitTypeName("狼人基地");
    contentSetting.setCore(core);
    
    UnderWorld::Core::UnitSetting tower;
    tower.setUnitTypeName("狼人箭塔");
    contentSetting.setTower(tower);
    
    set<string> cards;
    std::vector<UnderWorld::Core::CardSetting> cardSettings;
    UserDefaultsDataManager::getInstance()->getSelectedCards(cards);
    
    int i = 0;
    for (auto iter = begin(cards); iter != end(cards); ++iter, ++i) {
        UnderWorld::Core::CardSetting cs;
        cs.setCardTypeName(*iter);
        cardSettings.push_back(cs);
    }
    contentSetting.setCards(cardSettings);
    
    _test = new UnderworldTestPvpClient();
    _test->init(_render);
    _test->startTest(contentSetting);
}

void BattleScene::clear()
{
    if (_client) {
        _client->quit();
    }
    CC_SAFE_DELETE(_client);
    CC_SAFE_DELETE(_sch);
    CC_SAFE_DELETE(_render);
    removeAllChildren();
}
