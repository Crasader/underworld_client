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
#include "UnitType.h"
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
#include "CCShake.h"
#include "BulletType.h"

using namespace std;
using namespace UnderWorld::Core;

static const string tickSelectorKey("tickSelectorKey");
static const int battleTotalTime(600);

GameRender::GameRender(Scene* scene, int mapId, const string& mapData, const string& opponentsAccount)
:_observer(nullptr)
,_scene(scene)
,_mapId(mapId)
,_mapLayer(nullptr)
,_mapUILayer(nullptr)
,_game(nullptr)
,_commander(nullptr)
#if ENABLE_DRAG_CARD
,_selectedCamp(nullptr)
#endif
,_paused(false)
,_isGameOver(false)
,_remainingTime(battleTotalTime)
,_goldCount(INVALID_VALUE)
,_woodCount(INVALID_VALUE)
,_hasUpdatedBattleCampInfos(false)
{
    _mapLayer = MapLayer::create(mapId, mapData);
    _mapLayer->registerObserver(this);
    scene->addChild(_mapLayer);
    
    _mapUILayer = MapUILayer::create("Warewolf", opponentsAccount);
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

const MapSetting& GameRender::getMapSetting() const
{
    return _mapLayer->getMapSetting();
}

void GameRender::init(const Game* game, Commander* commander)
{
    _game = game;
    _commander = commander;
    
    // get cores
    const World* world = game->getWorld();
    world = game->getWorld();
    if (world) {
        const int factionsCount = world->getFactionCount();
        for (int i = 0; i < factionsCount; ++i) {
            const Faction* faction = world->getFaction(i);
            _cores.insert(make_pair(faction->getFactionIndex(), faction->findFirstUnitByClass(kUnitClass_Core)));
        }
        
        const int factionIndex = world->getThisFactionIndex();
        const int campsCount = world->getCampCount(factionIndex);
        for (int i = 0; i < campsCount; ++i) {
            const Camp* camp = world->getCamp(factionIndex, i);
            UnitClass uc = camp->getCurrentUnitType()->getUnitClass();
            if (_myCamps.find(uc) == _myCamps.end()) {
                _myCamps.insert(make_pair(uc, vector<const Camp*>()));
            }
            _myCamps.at(uc).push_back(camp);
        }
    }
    
    updateAll();
    
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
            updateAll();
            
            if (_mapUILayer) {
                _mapUILayer->reload();
            }
        }
    }
}

void GameRender::updateAll()
{
    // update units
    for (int i = 0; i < _game->getWorld()->getFactionCount(); ++i) {
        updateUnits(_game, i);
    }
    
    // update bullets
    updateBullets(_game);
    
    // update ui layer
    updateUILayer();
}

void GameRender::updateUnits(const Game* game, int index)
{
    const World* world = game->getWorld();

    const Faction* f = world->getFaction(index);
    for (int i = 0; i < f->getUnitCount(); ++i) {
        const Unit* unit = f->getUnitByIndex(i);
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
                        // add existent hero
                        const UnitType* unitType = unit->getUnitBase().getUnitType();
                        const UnitClass unitClass = unitType->getUnitClass();
                        if (kUnitClass_Building != unitClass && kUnitClass_Core != unitClass) {
                            _mapLayer->addPlaceUnitEffect(pos);
                            
                            if (kUnitClass_Hero == unitClass) {
                                const string& unitName = unitType->getName();
                                if (_myHeroes.find(unitName) == _myHeroes.end()) {
                                    _myHeroes.insert(make_pair(unitName, map<int, const Unit*>()));
                                }
                                
                                map<int, const Unit*>& m = _myHeroes.at(unitName);
                                if (m.find(key) == m.end()) {
                                    m.insert(make_pair(key, unit));
                                }
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
                const pair<Coordinate, float>& params = _bulletParams.at(key);
                const Coordinate& opos = params.first;
                const float h = params.second;
                const float d = sqrt(pow(abs(opos.x- targetPos.x), 2) + pow(abs(opos.y - targetPos.y), 2));
                const float distance = sqrt(pow(abs(pos.x- opos.x), 2) + pow(abs(pos.y - opos.y), 2));
                float height = 0;
                const float bulletMaxHeightFactor = bullet->getBulletType()->getHeight();
                if (d > 0) {
                    const float a = - (2.0f * d * bulletMaxHeightFactor + h + 2.0f * sqrt(pow(d * bulletMaxHeightFactor, 2) + d * h * bulletMaxHeightFactor)) / pow(d, 2);
                    const float b = 2.0f * (d * bulletMaxHeightFactor + sqrt(pow(d * bulletMaxHeightFactor, 2) + d * h * bulletMaxHeightFactor)) / d;
                    height = a * pow(distance, 2) + b * distance + h;
                }
                _mapLayer->repositionUnit(node, pos + Coordinate(0, height));
            }
        } else {
            if (!isExploded) {
                BulletNode* node = BulletNode::create(bullet);
                node->registerObserver(this);
                const float height = bullet->getHeight();
                _mapLayer->addUnit(node, pos + Coordinate(0, height));
                _allBulletNodes.insert(make_pair(key, node));
                _bulletParams.insert(make_pair(key, make_pair(pos, height)));
            }
        }
    }
}

void GameRender::updateUILayer()
{
    if (!_mapUILayer) {
        assert(false);
        return;
    }
    
    updateResources();
    
    const World* world = _game->getWorld();
    if (world) {
        for (auto iter = begin(_cores); iter != end(_cores); ++iter) {
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
    
    _mapUILayer->updateRemainingTime(_remainingTime);
}

bool GameRender::isCampFull(const Camp* camp) const
{
    if (camp) {
        const int production = camp->getProduction();
        if (production >= camp->getMaxProduction()) {
            return true;
        } else {
            if (kUnitClass_Hero == camp->getCurrentUnitType()->getUnitClass() && production >= 1) {
                return true;
            }
        }
    }
    
    return false;
}

bool GameRender::isValidAoeSpell(const Spell* spell) const
{
    if (spell) {
        const SpellType* spellType = spell->getSpellType();
        if (spellType) {
            SpellCastType castType = spellType->getCastType();
            if (kSpellCastType_Position == castType || kSpellCastType_PositionOrUnit == castType) {
                if (spell->getCDProgress() <= 0) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool GameRender::isProducibleCamp(const Camp* camp) const
{
    const int production = camp->getProduction();
    const float cd = camp->getColdDown();
    if (production > 0 && cd <= 0) {
        if (onMapUILayerIsHeroAlive(camp)) {
            return false;
        }
        
        return true;
    }
    
    return false;
}

const Spell* GameRender::getSpell(const Camp* camp, int idx) const
{
    if (camp) {
        const Unit* hero = camp->getHero();
        if (hero && hero->getSpellCount() > idx) {
            return hero->getSpellByIndex(idx);
        }
    }
    
    return nullptr;
}

CommandResult GameRender::castSpell(const Spell* spell, const Unit* trigger, const Coordinate& coordinate, const Unit* target)
{
    if (spell && trigger) {
        const string& spellAlias = spell->getSpellType()->getAlias();
        return _commander->tryGiveUnitCommand(trigger, kCommandClass_Cast, &coordinate, target, spellAlias);
    }
    
    return kCommandResult_failed;
}

UnitNode* GameRender::getHeroUnitNode(const Camp* camp)
{
    const Unit* trigger = camp->getHero();
    if (trigger) {
        const int unitId = trigger->getUnitId();
        if (_allUnitNodes.find(unitId) != _allUnitNodes.end()) {
            UnitNode* node = _allUnitNodes.at(unitId);
            return node;
        }
    }
    
    return nullptr;
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
    
}

void GameRender::onUnitNodePlayDeadAnimationFinished(int unitId)
{
    removeUnit(unitId);
}

void GameRender::onUnitNodeHurtTheTarget(UnitNode* node)
{
    const Unit* unit = node->getUnit();
    hurtUnit(unit->getTarget(), unit->getUnitBase().getRenderKey());
}

void GameRender::onUnitNodeShakeScreen(UnitNode* node)
{
    if (_scene) {
        static float shake_duration = 0.4f;
        static float shake_strength = 10.0f;
        static float shake_action_tag = 2016;
        
        CCShake* shakeAction = CCShake::actionWithDuration(shake_duration, shake_strength, Point::ZERO);
        shakeAction->setTag(shake_action_tag);
        _scene->stopActionByTag(shake_action_tag);
        _scene->runAction(shakeAction);
    }
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

#pragma mark - MapLayerObserver
void GameRender::onMapLayerTouchMoved(const Point& point)
{
#if false
    if (_selectedCamp) {
        onMapUILayerTouchMoved(_selectedCamp, point);
    }
#endif
}

void GameRender::onMapLayerTouchEnded(const Point& point)
{
#if ENABLE_DRAG_CARD
    if (_selectedCamp) {
        onMapUILayerTouchEnded(_selectedCamp, convertToUILayer(point));
    }
#endif
}

#pragma mark - MapUILayerObserver
bool GameRender::onMapUILayerIsGameOver() const
{
    return _isGameOver;
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

bool GameRender::onMapUILayerIsHeroAlive(const Camp* camp) const
{
    const UnitClass uc = camp->getCurrentUnitType()->getUnitClass();
    if (kUnitClass_Hero == uc) {
        const Unit* hero = camp->getHero();
        if (hero && hero->isAlive()) {
            return true;
        }
    }
    
    return false;
}

const vector<const Camp*>& GameRender::onMapUILayerGetCamps() const
{
    if (_myCamps.find(kUnitClass_Warrior) != _myCamps.end()) {
        return _myCamps.at(kUnitClass_Warrior);
    }
    
    static vector<const Camp*> empty;
    return empty;
}

void GameRender::onMapUILayerUnitSelected(const Camp* camp)
{
    _selectedCamp = camp;
}

void GameRender::onMapUILayerUnitTouched(const Camp* camp)
{
#if ENABLE_DRAG_CARD
    const Spell* spell = getSpell(camp, 0);
    if (spell) {
        SpellCastType castType = spell->getSpellType()->getCastType();
        if (kSpellCastType_Self == castType) {
            castSpell(spell, camp->getHero(), Coordinate::ZERO, nullptr);
        }
    }
#else
    if (_commander && camp) {
        if (isCampFull(camp)) {
            const Spell* spell = getSpell(camp, 0);
            if (spell) {
                SpellCastType castType = spell->getSpellType()->getCastType();
                if (kSpellCastType_Self == castType) {
                    castSpell(spell, camp->getHero(), Coordinate::ZERO, nullptr);
                }
            }
        }
    }
#endif
}

void GameRender::onMapUILayerTouchCancelled(const Camp* camp)
{
    UnitNode* node = getHeroUnitNode(camp);
    if (node) {
        node->removeSpellRing();
    }
    
    if (_mapLayer) {
        _mapLayer->removeSpellRangeRing();
    }
}

void GameRender::onMapUILayerTouchMoved(const Camp* camp, const Point& point)
{
    const Point& realPos = convertToMapLayer(point);
    const Spell* spell = getSpell(camp, 0);
    if (isValidAoeSpell(spell)) {
        UnitNode* node = getHeroUnitNode(camp);
        if (node) {
            node->addSpellRing(spell->getSpellType()->getCastDistance());
        }
        
        if (_mapLayer) {
            _mapLayer->updateSpellRangeRing(realPos, 400);
        }
    } else if (isProducibleCamp(camp)) {
        if (_mapLayer) {
            _mapLayer->updateUnitMask(camp, realPos);
        }
    }
}

void GameRender::onMapUILayerTouchEnded(const Camp* camp, const Point& point)
{
    const Point& realPos = convertToMapLayer(point);
    const Spell* spell = getSpell(camp, 0);
    if (isValidAoeSpell(spell)) {
        const Coordinate& coordinate = _mapLayer->convertPoint(realPos);
        CommandResult result = castSpell(spell, camp->getHero(), coordinate, nullptr);
        if (kCommandResult_suc == result) {
            if (spell->getSpellName().find("火球术") != string::npos) {
                if (_mapLayer) {
                    _mapLayer->addFireballSpellEffect();
                }
            }
        }
    } else if (isProducibleCamp(camp)) {
        if (_mapLayer) {
            const Coordinate& coordinate = _mapLayer->convertPoint(point);
            CommandResult result = _commander->tryGiveCampGenerateCommand(camp, coordinate);
            if (kCommandResult_suc == result) {
                _mapUILayer->clearHighlightedCamp();
            }
        }
    }
    
    UnitNode* node = getHeroUnitNode(camp);
    if (node) {
        node->removeSpellRing();
    }
    
    if (_mapLayer) {
        _mapLayer->removeSpellRangeRing();
        _mapLayer->removeUnitMask();
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

#pragma mark - DefeatLayerObserver
void GameRender::onDefeatLayerClosed(Layer* pSender)
{
    
}

void GameRender::onDefeatLayerContinued(Layer* pSender)
{
    
}

#pragma mark - private
void GameRender::removeUnit(int unitId)
{
    if (_allUnitNodes.find(unitId) != _allUnitNodes.end()) {
        Node* node = _allUnitNodes.at(unitId);
        node->removeFromParent();
        _allUnitNodes.erase(unitId);
    }
    
    for (auto iter = begin(_myHeroes); iter != end(_myHeroes); ++iter) {
        map<int, const Unit*>& heroes = iter->second;
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
    for (auto iter = begin(_allBulletNodes); iter != end(_allBulletNodes); ++iter) {
        iter->second->removeFromParent();
    }
    
    _allBulletNodes.clear();
    _bulletParams.clear();
}

void GameRender::removeAllUnits()
{
    for (auto iter = begin(_allUnitNodes); iter != end(_allUnitNodes); ++iter) {
        iter->second->removeFromParent();
    }
    
    _allUnitNodes.clear();
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
    
    -- _remainingTime;
    
    if (_remainingTime <= 0) {
        _remainingTime = 0;
    }
}

void GameRender::updateResources()
{
    const World* world = _game->getWorld();
    if (world && _mapUILayer) {
        const TechTree* techTree = world->getTechTree();
        const Faction* faction = world->getThisFaction();
        const int count = techTree->getResourceTypeCount();
        for (int i = 0; i < count; ++i) {
            const UnderWorld::Core::ResourceType* resourceType = techTree->getResourceTypeByIndex(i);
            const Resource* resource = faction->getResource(resourceType);
            if (kResourceClass_holdable == resourceType->_class) {
                CCASSERT(false, "There is no population any more.");
            } else {
                const string& name = resourceType->_name;
                const float decimalCnt = resource->getBalanceFloat();
                if (name == RES_NAME_GOLD) {
                    if (_goldCount != decimalCnt) {
                        _goldCount = decimalCnt;
                    }
                } else if (name == RES_NAME_WOOD) {
                    if (_woodCount != decimalCnt) {
                        _woodCount = decimalCnt;
                    }
                }
            }
        }
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
    
    for (auto iter = begin(_allUnitNodes); iter != end(_allUnitNodes); ++iter) {
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
        // TODO: change "_mapId"
        if (win) {
            VictoryLayer* layer = VictoryLayer::create(1 /*_mapId*/);
            layer->registerObserver(this);
            _scene->addChild(layer);
        } else {
            DefeatLayer* layer = DefeatLayer::create(1 /*_mapId*/);
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


Point GameRender::convertToMapLayer(const Point& uiLayerPoint) const
{
    return _mapLayer->convertToNodeSpace(_mapUILayer->convertToWorldSpace(uiLayerPoint));
}

Point GameRender::convertToUILayer(const Point& mapLayerPoint) const
{
    return _mapUILayer->convertToNodeSpace(_mapLayer->convertToWorldSpace(mapLayerPoint));
}
