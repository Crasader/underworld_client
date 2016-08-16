//
//  GameManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/18.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GameManager.h"
#include "GameScheduler.h"
#include "UnderworldClient.h"
#include "ClientTCPNetworkProxy.h"
#include "GameSettings.h"
#include "TechTree.h"
#include "GameData.h"
#include "DataManager.h"
#include "FrameLoader.h"
#include "FormationData.h"
#include "CocosUtils.h"
#include "CheatConfiguration.h"
#include "LocalHelper.h"
#include "BattleScene.h"
#include "MainScene.h"
#include "MessageBoxLayer.h"
#include "UWRender.h"

using namespace std;

struct GameManager::BattleContent {
    UnderWorld::Core::GameContentSetting contentSetting;
    vector<int> cards;
    GameModeHMMSetting::InitUnitList unitList;
    vector<UnderWorld::Core::UnitSetting> unitPool;
};

static GameManager* s_pInstance(nullptr);
GameManager* GameManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) GameManager();
    }
    
    return s_pInstance;
}

void GameManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

GameManager::GameManager()
:_isLaunching(false)
,_isPvp(false)
,_mapId(0)
,_scene(nullptr)
,_render(nullptr)
,_gameScheduler(nullptr)
,_pvpProxy(nullptr)
,_client(nullptr)
,_battleContent(nullptr)
{}

GameManager::~GameManager()
{
    purgeClient();
    CC_SAFE_DELETE(_battleContent);
}

#pragma mark - public
void GameManager::launchPve(int mapId)
{
    _isPvp = false;
    _mapId = mapId;
    launchGame();
}

void GameManager::launchPvp()
{
    _isPvp = true;
    _mapId = 0;
    launchGame();
}

#pragma mark - GameRenderObserver
void GameManager::onGameRenderRestart()
{
    if (!_isPvp) {
        restartPve();
    }
}

void GameManager::onGameRenderExit()
{
    exitGame();
    CocosUtils::replaceScene(MainScene::create(), true);
}

#pragma mark - private
void GameManager::launchGame()
{
    if (_isLaunching) {
        return;
    }
    
    if (loadBattleContent()) {
        _isLaunching = true;
        _scene = BattleScene::create();
        _scene->retain();
        createClient(_scene);
        if (CheatConfiguration::getInstance()->loadPVR) {
            auto loader(FrameLoader::getInstance());
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            loader->addAllFramesAsync([this]() { onPVRLoaded(); });
#else
            loader->addAllFrames();
            onPVRLoaded();
#endif
        } else {
            onPVRLoaded();
        }
    } else {
        MessageBoxLayer::getInstance()->show(LocalHelper::getString("hint_selectNoCard"));
    }
}

void GameManager::exitGame()
{
    purgeClient();
    FrameLoader::getInstance()->removeCachedFrames();
    CocosUtils::cleanMemory();
}

void GameManager::restartPve()
{
    createClient(_scene);
    if (_client && _battleContent) {
        _client->launchPve(_mapId, _battleContent->contentSetting, _battleContent->cards, _battleContent->unitList, _battleContent->unitPool);
    }
}

void GameManager::createClient(Scene* scene)
{
    if (_client) {
        purgeClient();
    }
    
    // 1. render
    _render = new (nothrow) UWRender(scene);
//    _render = new (nothrow) GameRender(scene, "");
//    dynamic_cast<GameRender*>(_render)->registerObserver(this);
    
    // 2. scheduler
    _gameScheduler = new (nothrow) GameScheduler();
    
    // 3. proxy
    static const pair<string, int> host = {"192.168.31.139", 9999};
//    static const pair<string, int> host = {"123.57.221.242", 8888};
    _pvpProxy = new (nothrow) ClientTCPNetworkProxy(host.first, host.second, "demoplayer", GameData::getInstance()->getUUID());
    
    // 4. client
    _client = new (nothrow) UnderworldClient("mofish", _pvpProxy, _gameScheduler, _render);
}

void GameManager::purgeClient()
{
    if (_client) {
        _client->quit();
    }
    
    CC_SAFE_DELETE(_render);
    CC_SAFE_DELETE(_client);
    CC_SAFE_DELETE(_pvpProxy);
    CC_SAFE_DELETE(_gameScheduler);
}

bool GameManager::loadBattleContent()
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
    
    // reload formation data every time
    auto formationData = GameData::getInstance()->currentUser()->getDefaultFormationData();
    
    _battleContent->cards.clear();
    _battleContent->unitList.clear();
    
    if (formationData) {
        _battleContent->cards.clear();
        for (auto spellType : formationData->getSpells()) {
            _battleContent->cards.push_back(DataManager::getInstance()->getGameModeHMM()->findCardTypeById(spellType)->getId());
        }
        
        const auto& heroes = formationData->getHeroes();
        for (auto iter = begin(heroes); iter != end(heroes); ++iter) {
            static const Point basePosition(240, 120);
            const auto& coordinate = iter->first;
            
            pair<int, int> pair;
            pair.first = DataManager::getInstance()->getGameModeHMM()->findCardTypeById(iter->second)->getId();
            pair.second = coordinate.y * FORMATION_WIDTH + coordinate.x;
            
            _battleContent->unitList.push_back(pair);
        }
    }
    
    if (_battleContent->cards.empty()) {
        return false;
    }
    
    return true;
}

void GameManager::onPVRLoaded()
{
    _isLaunching = false;
    CocosUtils::replaceScene(_scene, true);
    _scene->release();
    if (_client && _battleContent) {
        if (_isPvp) {
            _client->launchPvp(_battleContent->contentSetting, _battleContent->cards, _battleContent->unitList, _battleContent->unitPool);
        } else {
            _client->launchPve(_mapId, _battleContent->contentSetting, _battleContent->cards, _battleContent->unitList, _battleContent->unitPool);
        }
    }
}
