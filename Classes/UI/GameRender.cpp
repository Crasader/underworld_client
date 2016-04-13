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
static const float spellRingRange(400);
static const int battleTotalTime(600);

GameRender::GameRender(Scene* scene, int mapId, const string& mapData, const string& opponentsAccount)
:_observer(nullptr)
,_scene(scene)
,_mapId(mapId)
,_mapLayer(nullptr)
,_mapUILayer(nullptr)
,_game(nullptr)
,_commander(nullptr)
,_deck(nullptr)
,_paused(false)
,_isGameOver(false)
,_remainingTime(battleTotalTime)
,_minPuttingX(0)
,_maxPuttingX(0)
{
    _mapLayer = MapLayer::create(mapId, mapData);
    _mapLayer->registerObserver(this);
    scene->addChild(_mapLayer);
    
    _mapUILayer = MapUILayer::create("Warewolf", opponentsAccount);
    _mapUILayer->registerObserver(this);
    scene->addChild(_mapUILayer);
    
    onMapUILayerCardSelected(nullptr, INVALID_VALUE);
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
    if (world) {
        const int factionsCount = world->getFactionCount();
        for (int i = 0; i < factionsCount; ++i) {
            const Faction* faction = world->getFaction(i);
            _cores.insert(make_pair(faction->getFactionIndex(), faction->findFirstUnitByClass(kUnitClass_Core)));
        }
    }
    
    const int factionIndex = world->getThisFactionIndex();
    _deck = game->getDeck(factionIndex);
    
    if (_mapUILayer) {
        const int count = _deck ? _deck->getHandCapacity() : 0;
        _mapUILayer->createCardDeck(count);
    }
    
    const Faction* faction = world->getThisFaction();
    if (faction) {
        const UnderWorld::Core::Rect32& rect = faction->getPuttingArea();
        _minPuttingX = rect._origin.x;
        _maxPuttingX = _minPuttingX + rect._width;
    }
    
    assert(_maxPuttingX > 0 && _minPuttingX < _maxPuttingX);
    
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
        const Coordinate32& pos = unit->getCenterPos();
        const Skill* skill = unit->getCurrentSkill();
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
                    
                    _mapLayer->checkUnitInSpellRing(node);
                }
            } else {
                if (kSkillClass_Die != sc) {
                    const int factionIndex = f->getFactionIndex();
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
        const Coordinate32& pos = bullet->getPos();
        const Coordinate32& targetPos = bullet->targetPos();
        const bool isExploded(bullet->isExploded());
        if (_allBulletNodes.find(bullet) != _allBulletNodes.end()) {
            // already exist, update it
            BulletNode* node = _allBulletNodes.at(bullet);
            node->update();
            if (isExploded) {
                const BulletType* bt = bullet->getBulletType();
                if (bt && _mapLayer) {
                    _mapLayer->addBulletExplosionEffect(bt->getRenderKey(), pos);
                }
                node->removeFromParent();
                _allBulletNodes.erase(bullet);
                _bulletParams.erase(bullet);
            } else {
                const pair<Coordinate32, float>& params = _bulletParams.at(bullet);
                const Coordinate32& opos = params.first;
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
                _mapLayer->repositionUnit(node, pos + Coordinate32(0, height));
            }
        } else {
            if (!isExploded) {
                BulletNode* node = BulletNode::create(bullet);
                node->registerObserver(this);
                const float height = bullet->getHeight();
                _mapLayer->addUnit(node, pos + Coordinate32(0, height));
                _allBulletNodes.insert(make_pair(bullet, node));
                _bulletParams.insert(make_pair(bullet, make_pair(pos, height)));
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
    
    if (_deck) {
        const int counter = _deck->getCounter();
        const int total = _deck->getDrawSpanFrames();
        const float duration = (float)total / (float)GameConstants::FRAME_PER_SEC;
        const float time = (1.0 - (float)(counter % total) / (float)total) * duration;
        _mapUILayer->updateCountDown(time, duration);
        
        const auto& logs = _deck->getLogs();
        for (int i = 0; i < logs.size(); ++i) {
            const auto& log = logs.at(i);
            const auto event = log._event;
            const auto card = log._card;
            if (Deck::kDeckEvent_Use == event) {
                _mapUILayer->removeCard(card, log._extra);
            } else if (Deck::kDeckEvent_Draw == event) {
                _mapUILayer->insertCard(card);
            }
        }
        
        _deck->clearEventLog();
    }
}

bool GameRender::isValidAoeSpell(const SpellType* spellType) const
{
    if (spellType) {
        const SpellCastType castType = spellType->getCastType();
        if (kSpellCastType_Position == castType || kSpellCastType_PositionOrUnit == castType) {
            return true;
        }
    }
    
    return false;
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
void GameRender::onMapLayerTouchBegan(const Point& point)
{
    const Card* card = _selectedCard.first;
    if (card) {
        updateCardMask(card, point, spellRingRange);
    }
}

void GameRender::onMapLayerTouchMoved(const Point& point, bool isValid)
{
    if (isValid) {
        const Card* card = _selectedCard.first;
        if (card) {
            updateCardMask(card, point, spellRingRange);
        }
    } else {
        removeCardMask();
    }
}

void GameRender::onMapLayerTouchEnded(const Point& point)
{
    const Card* card = _selectedCard.first;
    if (card) {
        updateCardMask(card, point, spellRingRange);
        tryToUseCard(card, _selectedCard.second, point);
    }
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

void GameRender::onMapUILayerCardSelected(const Card* card, int idx)
{
    _selectedCard.first = card;
    _selectedCard.second = idx;
    
    if (_mapLayer) {
        // clear first
        _mapLayer->clearUnplacedAreas();
        
        if (card) {
            const CardType* ct = card->getCardType();
            if (ct && kCardClass_Unit == ct->getCardClass()) {
                _mapLayer->setPlacedArea(_minPuttingX, _maxPuttingX);
            }
        }
    }
}

void GameRender::onMapUILayerTouchMoved(const Card* card, const Point& point)
{
    updateCardMask(card, convertToMapLayer(point), spellRingRange);
}

void GameRender::onMapUILayerTouchEnded(const Card* card, int idx, const Point& point)
{
    const Point& p = convertToMapLayer(point);
    updateCardMask(card, p, spellRingRange);
    tryToUseCard(card, idx, p);
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
    _cores.clear();
}

void GameRender::pauseGame()
{
    MessageBoxLayer::getInstance()->show(LocalHelper::getString("hint_exitPve"), MessageBoxType::YesNo, [](Ref*) {
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
    if (world) {
        const TechTree* techTree = world->getTechTree();
        const Faction* faction = world->getThisFaction();
        const int count = techTree->getResourceTypeCount();
        for (int i = 0; i < count; ++i) {
            const UnderWorld::Core::ResourceType* resourceType = techTree->getResourceTypeByIndex(i);
            const Resource* resource = faction->getResource(resourceType);
            if (kResourceClass_consumable == resourceType->_class) {
                const string& name = resourceType->_name;
                const float value = ((float)resource->getBalanceMicro()) / GameConstants::MICRORES_PER_RES;
                if (_resources.find(name) != end(_resources)) {
                    _resources.at(name) = value;
                } else {
                    _resources.insert(make_pair(name, value));
                }
            }
        }
        
        if (_mapUILayer) {
            _mapUILayer->updateResource(_resources);
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

void GameRender::updateCardMask(const UnderWorld::Core::Card* card, const Point& point, float range)
{
    if (_mapLayer && _mapUILayer) {
        const bool inDeck = _mapUILayer->isPointInTableView(convertToUILayer(point));
        if (inDeck) {
            removeCardMask();
        } else {
            const UnitType* ut = card->getUnitType();
            Coordinate32 coordinate = getValidPuttingCoordinate(point, ut);
            if (ut) {
                _mapLayer->updateUnitMask(ut, coordinate);
            }
        }
    }
}

void GameRender::removeCardMask()
{
    if (_mapLayer) {
        _mapLayer->removeSpellRing();
        _mapLayer->removeUnitMask();
    }
}

void GameRender::tryToUseCard(const UnderWorld::Core::Card* card, int idx, const Point& point)
{
    if (_mapLayer) {
        const bool inDeck = _mapUILayer->isPointInTableView(convertToUILayer(point));
        if (inDeck) {
            // TODO:
        } else if (_commander) {
            Coordinate32 coordinate = getValidPuttingCoordinate(point, true);
            CommandResult result = _commander->tryGiveDeckUseCommand(_deck, idx, coordinate);
            if (kCommandResult_suc == result) {
                _mapUILayer->clearHighlightedCard();
            }
        }
        
        _mapLayer->removeSpellRing();
        _mapLayer->removeUnitMask();
    }
}

Coordinate32 GameRender::getValidPuttingCoordinate(const Point& point, bool check) const
{
    if (_mapLayer) {
        Coordinate32 coordinate = _mapLayer->convertPoint(point);
        if (check) {
            const int x = coordinate.x;
            const int y = coordinate.y;
            if (x < _minPuttingX) {
                return Coordinate32(_minPuttingX, y);
            } else if (x >= _maxPuttingX) {
                return Coordinate32(_maxPuttingX - 1, y);
            }
        }
        
        return coordinate;
    }
    
    return Coordinate32(-1, -1);
}
