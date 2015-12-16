//
//  GameRender.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/30.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GameRender.h"
#include "Game.h"
#include "SoundManager.h"
#include "MapLayer.h"
#include "UnitNode.h"
#include "BulletNode.h"
#include "Map.h"
#include "Camp.h"
#include "VictoryLayer.h"
#include "DefeatLayer.h"

using namespace std;
using namespace UnderWorld::Core;

static const string tickSelectorKey("tickSelectorKey");
static const int waveTime(20);
static const int battleTotalTime(600);

GameRender::GameRender(Scene* scene, int mapId, MapLayer* mapLayer, const string& opponentsAccount)
:_observer(nullptr)
,_scene(scene)
,_mapId(mapId)
,_mapLayer(mapLayer)
,_mapUILayer(nullptr)
,_game(nullptr)
,_commander(nullptr)
,_paused(false)
,_isGameOver(false)
,_waveTime(11)  // TODO: reset wave time
,_remainingTime(battleTotalTime)
{    
    _mapUILayer = MapUILayer::create("我的名字", opponentsAccount);
    _mapUILayer->registerObserver(this);
    scene->addChild(_mapUILayer);
}

GameRender::~GameRender()
{
    stopAllTimers();
    removeAllUnits();
    removeAllBullets();
}

void GameRender::registerObserver(GameRenderObserver *observer)
{
    _observer = observer;
}

void GameRender::init(const Game* game, Commander* commander)
{
    _game = game;
    _commander = commander;
    
    // create units
    for (int i = 0; i < game->getWorld()->getFactionCount(); ++i) {
        updateUnits(game, i);
    }
    
    // create bullets
    updateBullets(game);
    
    // get cores
    const World* world = game->getWorld();
    world = game->getWorld();
    if (world) {
        const int cnt = world->getFactionCount();
        for (int i = 0; i < cnt; ++i) {
            const Faction* faction = world->getFaction(i);
            _cores.insert(make_pair(faction->getFactionIndex(), faction->findFirstUnitByClass(kUnitClass_Core)));
        }
    }
    
    if (_mapUILayer) {
        _mapUILayer->reload();
        initUILayer();
    }
    
    // tick
    Scheduler* scheduler = Director::getInstance()->getScheduler();
    scheduler->schedule(CC_CALLBACK_1(GameRender::tick, this), this, 1.0f, false, tickSelectorKey);
    SoundManager::getInstance()->playBackgroundMusic(StringUtils::format("sounds/music/music_battle.mp3"));
    SoundManager::getInstance()->playSound("sounds/effect/sound_10sec.mp3");
}

void GameRender::render(const Game* game)
{
    assert(game = _game);
    
    // make sure "onGameOver()" is callded only once
    if (!_isGameOver) {
        _isGameOver = _game->isGameOver();
        
        if (_isGameOver) {
            onGameOver();
        } else {
            // update units
            for (int i = 0; i < game->getWorld()->getFactionCount(); ++i) {
                updateUnits(game, i);
            }
            
            // update bullets
            updateBullets(game);
            
            // update ui layer
            updateUILayer();
        }
    }
}

void GameRender::updateUnits(const Game* game, int index)
{
    const World* world = game->getWorld();

    const Faction* f = world->getFaction(index);
    const vector<Unit*>& units = f->getAllUnits();
    for (int i = 0; i < units.size(); ++i) {
        const Unit* unit = units.at(i);
        const Coordinate& pos = unit->getCenterPos();
        const int key = unit->getUnitId();
        const Skill* skill = unit->getCurrentSkill();
        // TODO: remove test code
        if (skill || (skill && skill->getSkillState() == Skill::kSkillState_performing)) {
            SkillClass sc = skill->getSkillType()->getSkillClass();
            bool isNewCreated = (_allUnits.find(key) == _allUnits.end()) ? true : false;
            if (isNewCreated) {
                if (kSkillClass_Die != sc) {
                    UnitNode* node = UnitNode::create(unit);
                    node->registerObserver(this);
                    _mapLayer->addUnit(node, pos);
                    _allUnits.insert(make_pair(key, node));
                }
            } else {
                // already exist, update it
                UnitNode* node = _allUnits.at(key);
                node->update();
                switch (sc) {
                    case kSkillClass_Stop:
                        break;
                    case kSkillClass_Move:
                    {
                        _mapLayer->repositionUnit(node, pos);
                    }
                        break;
                    case kSkillClass_Attack:
                        break;
                    case kSkillClass_Cast:
                        break;
                    case kSkillClass_Die:
                        break;
                        
                    default:
                        break;
                }
            }
        }
    }
}

void GameRender::updateBullets(const Game* game)
{
    const World* world = game->getWorld();
    for (int i = 0; i < world->getBulletCount(); ++i) {
        const Bullet* bullet = world->getBullet(i);
        const Coordinate& pos = bullet->getPos();
        const int64_t key = reinterpret_cast<int64_t>(bullet);
        const bool isExploded(bullet->isExploded());
        if (_allBullets.find(key) != _allBullets.end()) {
            // already exist, update it
            BulletNode* node = _allBullets.at(key);
            node->update();
            if (isExploded) {
                node->removeFromParent();
                _allBullets.erase(key);
            } else {
                _mapLayer->repositionUnit(node, pos);
            }
        } else {
            if (!isExploded) {
                BulletNode* node = BulletNode::create(bullet);
                node->registerObserver(this);
                _mapLayer->addUnit(node, pos);
                _allBullets.insert(make_pair(key, node));
            }
        }
    }
}

void GameRender::updateUILayer()
{
    updateResources();
    
    const World* world = _game->getWorld();
    if (world) {
        for (map<int, Unit*>::iterator iter = _cores.begin(); iter != _cores.end(); ++iter) {
            const Unit* core(iter->second);
            if (core) {
                const int maxHp = core->getUnitType()->getMaxHp();
                const int hp = core->getHp();
                const float percentage = 100 * (float)hp / (float)maxHp;
                if (iter->first == world->getThisFactionIndex()) {
                    _mapUILayer->updateMyHpProgress(percentage);
                } else {
                    _mapUILayer->updateOpponentsHpProgress(percentage);
                }
            }
        }
    }
}

void GameRender::hurtUnit(const Unit* target, const string& trigger)
{
    if (target && kSkillClass_Die != target->getCurrentSkill()->getSkillType()->getSkillClass()) {
        const int key = target->getUnitId();
        if (_allUnits.find(key) != _allUnits.end()) {
            UnitNode* node = _allUnits.at(key);
            if (node) {
                node->onHurt();
                node->addCritEffect(trigger);
            }
        }
    }
}

#pragma mark - UnitNodeObserver
bool GameRender::isUnitNodeBornOnTheRight(UnitNode* node)
{
    const Faction* faction = node->getUnit()->getBelongFaction();
    if (faction->getFactionIndex() != 0) {
        return true;
    }
    return false;
}

void GameRender::onUnitNodePlayDeadAnimationFinished(UnitNode* node)
{
    node->removeFromParent();
    _allUnits.erase(node->getUnit()->getUnitId());
}

void GameRender::onUnitNodeHurtTheTarget(UnitNode* node)
{
    const Unit* unit = node->getUnit();
    hurtUnit(unit->getTarget(), unit->getUnitType()->getName());
    node->addSwordEffect();
}

#pragma mark - BulletNodeObserver
void GameRender::onBulletNodeReachedTarget(BulletNode* node)
{
    
}

void GameRender::onBulletNodeExploded(BulletNode* node)
{
    const Bullet* bullet = node->getBullet();
    hurtUnit(bullet->getTarget(), bullet->getTrigger()->getUnitType()->getName());
}

#pragma mark - MapUILayerObserver
void GameRender::onMapUILayerUnitSelected(MapUIUnitNode* node)
{
    if (_commander) {
        int index = (int)node->getIdx();
        const World* world = _game->getWorld();
        const int factionIndex = world->getThisFactionIndex();
        if (index < world->getCampCount(factionIndex)) {
            const Camp* camp = world->getCamp(factionIndex, index);
            if (camp) {
                CommandResult result = _commander->tryGiveCampCommand(camp, 1);
                if (kCommandResult_suc == result) {
                    // TODO: replace the temp code with callback from camp
                    Scheduler *s = Director::getInstance()->getScheduler();
                    if (s) {
                        static string key("temp_test_update_MapUIUnitNode");
                        s->schedule([=](float dt) {
                            s->unschedule(key, this);
                            node->update(false);
                        }, this, 0.1f, false, key);
                    }
                }
            }
        }
    }
}

void GameRender::onMapUILayerClickedPauseButton()
{
    _paused = !_paused;
    if (_paused) {
        pauseGame();
    } else {
        resumeGame();
    }
}

ssize_t GameRender::onMapUILayerCampsCount()
{
    const World* world = _game->getWorld();
    const int factionIndex = world->getThisFactionIndex();
    return world->getCampCount(factionIndex);
}

const UnderWorld::Core::Camp* GameRender::onMapUILayerCampAtIndex(ssize_t idx)
{
    const World* world = _game->getWorld();
    const int factionIndex = world->getThisFactionIndex();
    const Camp* camp = world->getCamp(factionIndex, (int)idx);
    return camp;
}

#pragma mark - VictoryLayerObserver
void GameRender::onVictoryLayerClosed(Layer* pSender)
{
    
}

void GameRender::onVictoryLayerContinued(Layer* pSender)
{
    // remove layer first
    pSender->removeFromParent();
    if (_observer) {
        _observer->onGameRenderRestart();
    }
}

void GameRender::removeAllBullets()
{
    for (map<int64_t, BulletNode*>::iterator iter = _allBullets.begin(); iter != _allBullets.end(); ++iter) {
        iter->second->removeFromParent();
    }
    
    _allBullets.clear();
}

void GameRender::removeAllUnits()
{
    for (map<int, UnitNode*>::iterator iter = _allUnits.begin(); iter != _allUnits.end(); ++iter) {
        iter->second->removeFromParent();
    }
    
    _allUnits.clear();
}

void GameRender::pauseGame()
{
    
}

void GameRender::resumeGame()
{
    
}

void GameRender::restartGame()
{
    
}

void GameRender::tick(float dt)
{
    if (_isGameOver) {
        return;
    }
    
    -- _waveTime;
    -- _remainingTime;
    
    if (_waveTime <= 0) {
        _waveTime = waveTime;
    }
    
    if (_remainingTime <= 0) {
        _remainingTime = 0;
    }
    
    if (_mapUILayer) {
        _mapUILayer->updateWaveTime(_waveTime);
        _mapUILayer->updateRemainingTime(_remainingTime);
    }
}

void GameRender::updateResources()
{
    const World* world = _game->getWorld();
    if (world && _mapUILayer) {
        const TechTree* techTree = world->getTechTree();
        const Faction* faction = world->getThisFaction();
        int count = techTree->getResourceTypeCount();
        for (int i = 0; i < count; ++i) {
            const UnderWorld::Core::ResourceType* resourceType = techTree->getResourceTypeByIndex(i);
            const Resource* resource = faction->getResource(resourceType);
            if (kResourceClass_holdable == resourceType->_class) {
                _mapUILayer->updatePopulation(resource->getOccpied(), resource->getBalance());
            } else {
                _mapUILayer->updateGold(resource->getBalance());
            }
        }
    }
}

void GameRender::initUILayer()
{
    if (_mapUILayer) {
        _mapUILayer->updateWaveTime(_waveTime);
        _mapUILayer->updateRemainingTime(_remainingTime);
        _mapUILayer->updateMyHpProgress(100);
        _mapUILayer->updateOpponentsHpProgress(100);
        updateResources();
    }
}

void GameRender::stopAllTimers()
{
    Scheduler* scheduler = Director::getInstance()->getScheduler();
    scheduler->unschedule(tickSelectorKey, this);
}

void GameRender::onGameOver()
{
    stopAllTimers();
    removeAllBullets();
    
    bool win(false);
    const Unit* myCore = _cores.at(_game->getWorld()->getThisFactionIndex());
    if (myCore && myCore->isAlive()) {
        win = true;
    }
    
    for (map<int, UnitNode*>::iterator iter = _allUnits.begin(); iter != _allUnits.end(); ++iter) {
        UnitNode* node = iter->second;
        const int factionIndex = node->getUnit()->getBelongFaction()->getFactionIndex();
        if (factionIndex == _game->getWorld()->getThisFactionIndex()) {
            if (win) {
                node->onWin();
            } else {
                node->onLose();
            }
        } else {
            if (win) {
                node->onLose();
            } else {
                node->onWin();
            }
        }
    }
    
    // pop layer
    if (_scene) {
        if (win) {
            VictoryLayer* layer = VictoryLayer::create(_mapId);
            layer->registerObserver(this);
            _scene->addChild(layer);
        } else {
            VictoryLayer* layer = VictoryLayer::create(_mapId);
            layer->registerObserver(this);
            _scene->addChild(layer);
        }
    }
    
    // audio effect
    string audioFile;
    if (win) {
        audioFile = "sound_victory";
    } else {
        audioFile = "sound_failed";
    }
    SoundManager::getInstance()->playSound("sounds/effect/" + audioFile + ".mp3");
}