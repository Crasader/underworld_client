//
//  GameRender.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/30.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GameRender.h"
#include "Game.h"
#include "Map.h"
#include "Camp.h"
#include "Unit.h"
#include "Spell.h"
#include "SoundManager.h"
#include "MapLayer.h"
#include "UnitNode.h"
#include "BulletNode.h"
#include "CocosUtils.h"
#include "VictoryLayer.h"
#include "DefeatLayer.h"
#include "MainLayer.h"
#include "MessageBoxLayer.h"

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
        Unit* unit = units.at(i);
        const int key = unit->getUnitId();
        const Coordinate& pos = unit->getCenterPos();
        const Skill* skill = unit->getCurrentSkill();
        // TODO: remove test code
        if (skill) {
            SkillClass sc = skill->getSkillType()->getSkillClass();
            if (_allUnitNodes.find(key) != _allUnitNodes.end()) {
                // already exist, update it
                UnitNode* node = _allUnitNodes.at(key);
                node->update();
                if (_mapLayer) {
                    if (kSkillClass_Move == sc) {
                        _mapLayer->repositionUnit(node, pos);
                    }
                    _mapLayer->checkUnitInSpellRangeRing(node);
                }
            } else {
                if (kSkillClass_Die != sc) {
                    const int factionIndex = unit->getBelongFaction()->getFactionIndex();
                    // TODO: check if the unit is on the right
                    UnitNode* node = UnitNode::create(unit, factionIndex != 0);
                    node->registerObserver(this);
                    _mapLayer->addUnit(node, pos);
                    _allUnitNodes.insert(make_pair(key, node));
                    
                    if (factionIndex == world->getThisFactionIndex()) {
                        // add existent unit
                        if (_myUnits.find(key) == _myUnits.end()) {
                            _myUnits.insert(make_pair(key, unit));
                        }
                        
                        // add existent hero
                        const UnitType* unitType = unit->getUnitBase().getUnitType();
                        const UnitClass unitClass = unitType->getUnitClass();
                        if (kUnitClass_Hero == unitClass) {
                            const string& unitName = unitType->getName();
                            if (_myHeroes.find(unitName) == _myHeroes.end()) {
                                _myHeroes.insert(make_pair(unitName, map<int, Unit*>()));
                            }
                            
                            map<int, Unit*>& m = _myHeroes.at(unitName);
                            if (m.find(key) == m.end()) {
                                m.insert(make_pair(key, unit));
                            }
                        }
                    }
                } else {
#if COCOS2D_DEBUG
                    if (_allUnitNodes.find(key) != _allUnitNodes.end()) {
                        assert(false);
                        removeUnit(key);
                    }
#endif
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
        const Coordinate& targetPos = bullet->targetPos();
        const float distance = sqrt(pow(abs(pos.x- targetPos.x), 2) + pow(abs(pos.y - targetPos.y), 2));
        const int64_t key = reinterpret_cast<int64_t>(bullet);
        const bool isExploded(bullet->isExploded());
        if (_allBulletNodes.find(key) != _allBulletNodes.end()) {
            // already exist, update it
            BulletNode* node = _allBulletNodes.at(key);
            node->update();
            if (isExploded) {
                node->removeFromParent();
                _allBulletNodes.erase(key);
                _bulletParams.erase(key);
            } else {
                const pair<float, float>& params = _bulletParams.at(key);
                const float d1 = params.first;
                const float h1 = params.second;
                const float height = h1 * distance / d1;
                _mapLayer->repositionUnit(node, pos + Coordinate(0, height));
            }
        } else {
            if (!isExploded) {
                BulletNode* node = BulletNode::create(bullet);
                node->registerObserver(this);
                const float height = bullet->getHeight();
                _mapLayer->addUnit(node, pos + Coordinate(0, height));
                _allBulletNodes.insert(make_pair(key, node));
                _bulletParams.insert(make_pair(key, make_pair(distance, height)));
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
                const int maxHp = core->getUnitBase().getMaxHp();
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

bool GameRender::isCampFull(const Camp* camp) const
{
    if (camp) {
        const int production = camp->getProduction();
        if (production >= camp->getMaxProduction()) {
            return true;
        } else {
            const UnitType* unitType = camp->getUnitType();
            UnitClass unitClass = unitType->getUnitClass();
            if (kUnitClass_Hero == unitClass && production >= 1) {
                return true;
            }
        }
    }
    
    return false;
}

Spell* GameRender::getSpell(const Camp* camp, int idx, Unit** trigger) const
{
    if (camp) {
        const string& unitName = camp->getUnitSetting().getUnitTypeName();
        if (_myHeroes.find(unitName) != _myHeroes.end()) {
            const map<int, Unit*>& heroes = _myHeroes.at(unitName);
            if (heroes.size() > 0) {
                Unit* hero = heroes.begin()->second;
                if (hero) {
                    const int cnt = hero->getSpellCount();
                    if (idx < cnt) {
                        *trigger = hero;
                        return hero->getSpellByIndex(idx);
                    }
                }
            }
        }
    }
    
    return nullptr;
}

SpellCastType GameRender::getSpellCastType(const Camp* camp, int idx) const
{
    Unit* trigger(nullptr);
    Spell* spell = getSpell(camp, idx, &trigger);
    if (spell) {
        const SpellCastType castType = spell->getSpellType()->getCastType();
        return castType;
    }
    
    return SPELL_CAST_TYPE_COUNT;
}

bool GameRender::canCastSpell(const Camp* camp, string& spellAlias, Unit** trigger) const
{
    Spell* spell = getSpell(camp, 0, trigger);
    if (spell) {
        const SpellType* sp = spell->getSpellType();
        if (sp) {
            Unit* hero = *trigger;
            if (hero) {
                spellAlias = sp->getAlias();
                const int cd = spell->getCDProgress();
                if (cd <= 0) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void GameRender::castSpell(const Camp* camp, const Coordinate& coordinate, Unit* target)
{
    string spellAlias;
    Unit* trigger(nullptr);
    if (canCastSpell(camp, spellAlias, &trigger)) {
        CastCommand* command = dynamic_cast<CastCommand*>(Command::create(kCommandClass_Cast, coordinate, target));
        if (command) {
            command->setSpellAlias(spellAlias);
            if (trigger) {
                trigger->giveCommand(command);
            } else {
                assert(false);
            }
        }
    }
}

void GameRender::hurtUnit(const Unit* target, const string& trigger)
{
    if (target && kSkillClass_Die != target->getCurrentSkill()->getSkillType()->getSkillClass()) {
        const int key = target->getUnitId();
        if (_allUnitNodes.find(key) != _allUnitNodes.end()) {
            UnitNode* node = _allUnitNodes.at(key);
            if (node) {
                node->onHurt(trigger);
            }
        }
    }
}

#pragma mark - UnitNodeObserver
void GameRender::onUnitNodeUpdatedFeatures(int unitId)
{
    if (_myUnits.find(unitId) != _myUnits.end()) {
        Unit* unit = _myUnits.at(unitId);
        unit->clearEventLogs();
    }
}

void GameRender::onUnitNodePlayDeadAnimationFinished(int unitId)
{
    removeUnit(unitId);
}

void GameRender::onUnitNodeHurtTheTarget(UnitNode* node)
{
    const Unit* unit = node->getUnit();
    hurtUnit(unit->getTarget(), unit->getUnitBase().getRenderKey());
    node->addSwordEffect();
}

#pragma mark - BulletNodeObserver
void GameRender::onBulletNodeReachedTarget(BulletNode* node)
{
    
}

void GameRender::onBulletNodeExploded(BulletNode* node)
{
    const Bullet* bullet = node->getBullet();
    hurtUnit(bullet->getTarget(), bullet->getTrigger()->getUnitBase().getRenderKey());
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
                if (isCampFull(camp)) {
                    SpellCastType castType = getSpellCastType(camp, 0);
                    if (kSpellCastType_Self == castType) {
                        castSpell(camp, Coordinate::ZERO, nullptr);
                    }
                } else {
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

const Camp* GameRender::onMapUILayerCampAtIndex(ssize_t idx)
{
    const World* world = _game->getWorld();
    const int factionIndex = world->getThisFactionIndex();
    const Camp* camp = world->getCamp(factionIndex, (int)idx);
    return camp;
}

void GameRender::onMapUILayerSpellRingCancelled()
{
    if (_mapLayer) {
        _mapLayer->removeSpellRangeRing();
    }
}

void GameRender::onMapUILayerSpellRingMoved(const Camp* camp, const Point& position)
{
    SpellCastType castType = getSpellCastType(camp, 0);
    if (kSpellCastType_Position == castType ||
        kSpellCastType_PositionOrUnit == castType) {
        string spellAlias;
        Unit* trigger(nullptr);
        if (canCastSpell(camp, spellAlias, &trigger)) {
            if (_mapLayer) {
                const Point& realPos = _mapLayer->convertToNodeSpace(_mapUILayer->convertToWorldSpace(position));
                _mapLayer->updateSpellRangeRing(realPos);
            }
        }
    }
}

void GameRender::onMapUILayerTryToCastSpell(const Camp* camp, const Point& position)
{
    Unit* trigger(nullptr);
    Spell* spell = getSpell(camp, 0, &trigger);
    if (spell) {
        SpellCastType castType = spell->getSpellType()->getCastType();
        if (kSpellCastType_Position == castType) {
            Coordinate coordinate = _mapLayer->convertPoint(position);
            castSpell(camp, coordinate, nullptr);
            if (spell->getSpellName().find("火球术") != string::npos) {
                if (_mapLayer) {
                    _mapLayer->addFireballSpellEffect();
                }
            }
        }
    }
    
    if (_mapLayer) {
        _mapLayer->removeSpellRangeRing();
    }
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

#pragma mark - private
void GameRender::removeUnit(int unitId)
{
    if (_allUnitNodes.find(unitId) != _allUnitNodes.end()) {
        Node* node = _allUnitNodes.at(unitId);
        node->removeFromParent();
        _allUnitNodes.erase(unitId);
    }
    
    _myUnits.erase(unitId);
    
    for (map<string, map<int, Unit*>>::iterator iter = _myHeroes.begin(); iter != _myHeroes.end(); ++iter) {
        map<int, Unit*>& heroes = iter->second;
        if (heroes.find(unitId) != heroes.end()) {
            heroes.erase(unitId);
            if (heroes.size() == 0) {
                _myHeroes.erase(iter);
            }
            break;
        }
    }
}

void GameRender::removeAllBullets()
{
    for (map<int64_t, BulletNode*>::iterator iter = _allBulletNodes.begin(); iter != _allBulletNodes.end(); ++iter) {
        iter->second->removeFromParent();
    }
    
    _allBulletNodes.clear();
    _bulletParams.clear();
}

void GameRender::removeAllUnits()
{
    for (map<int, UnitNode*>::iterator iter = _allUnitNodes.begin(); iter != _allUnitNodes.end(); ++iter) {
        iter->second->removeFromParent();
    }
    
    _allUnitNodes.clear();
    _myUnits.clear();
    _myHeroes.clear();
    _cores.clear();
}

void GameRender::pauseGame()
{
    MessageBoxLayer::getInstance()->show(LocalHelper::getString("hint_exitPve"), kMessageBoxYesNo, [](Ref*) {
        CocosUtils::replaceScene(MainLayer::createScene());
    });
}

void GameRender::resumeGame()
{
    pauseGame();
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
                if (resourceType->_name == RES_NAME_GOLD) {
                    _mapUILayer->updateGold(resource->getBalance());
                }
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
    
    for (map<int, UnitNode*>::iterator iter = _allUnitNodes.begin(); iter != _allUnitNodes.end(); ++iter) {
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