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
#include "GameModeHMM.h"

using namespace std;
using namespace UnderWorld::Core;

static const string tickSelectorKey("tickSelectorKey");
static const int battleTotalTime(600);
static const float eyeRadians = tan(CC_DEGREES_TO_RADIANS(75));

GameRender::GameRender(Scene* scene, const string& opponentsAccount)
:_observer(nullptr)
,_scene(scene)
,_mapId(0)
,_opponentsAccount(opponentsAccount)
,_mapLayer(nullptr)
,_mapUILayer(nullptr)
,_game(nullptr)
,_world(nullptr)
,_techTree(nullptr)
,_gameModeHMM(nullptr)
,_commander(nullptr)
,_deck(nullptr)
,_paused(false)
,_isGameOver(false)
,_remainingTime(battleTotalTime)
,_minPuttingX(0)
,_maxPuttingX(0)
{
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

const Size& GameRender::getMapSize() const
{
    if (_mapLayer) {
        return _mapLayer->getMapSize();
    }
    
    return Size::ZERO;
}

Coordinate32 GameRender::point2Coordinate(const Point& point) const
{
    if (_mapLayer) {
        return _mapLayer->point2Coordinate(point);
    }
    
    return Coordinate32::ZERO;
}

void GameRender::init(const Game* game, Commander* commander)
{
    _game = game;
    _world = game->getWorld();
    _techTree = _world->getTechTree();
    _gameModeHMM = dynamic_cast<const GameModeHMM*>(game->getGameMode());
    _commander = commander;
    
    // deck
    const int thisFi = game->getThisFactionIndex();
    _deck = _gameModeHMM->getDeck(thisFi);
    
    const Rect32& rect = _deck->getSummonRegion();
    _minPuttingX = rect._origin.x;
    _maxPuttingX = _minPuttingX + rect._width;
    assert(_maxPuttingX > 0 && _minPuttingX < _maxPuttingX);
    
    // cores
    for (int i = 0; i < _world->getFactionCount(); ++i) {
        const Faction* faction = _world->getFaction(i);
        if (faction) {
            const int fi = faction->getFactionIndex();
            _cores.insert(make_pair(fi, faction->findFirstUnitByClass(kUnitClass_Core)));
        }
    }
    
    // UI
    _mapId = _world->getMap()->getMapId();
    
    _mapLayer = MapLayer::create(_mapId);
    _mapLayer->registerObserver(this);
    _scene->addChild(_mapLayer);
    
    _mapUILayer = MapUILayer::create("Warewolf", _opponentsAccount);
    _mapUILayer->registerObserver(this);
    _scene->addChild(_mapUILayer);
    
    onMapUILayerCardSelected("", INVALID_VALUE);
    
    if (_mapUILayer && _deck) {
        static const CardDeckType type(CardDeckType::Unit);
        const int count = _deck->getHandCount();
        _mapUILayer->createCardDeck(type, count);
        
        for (int i = 0; i < count; ++i) {
            const HMMCard* card = _deck->getHandCard(i);
            if (card) {
                const HMMCardType* ct = card->getCardType();
                if (ct) {
                    _mapUILayer->insertCard(type, card /*ct->getName()*/, false);
                    _handCards.push_back(ct->getName());
                }
            }
        }
#if false
        // spell cards
        const auto& names = getSpells();
        const auto cnt = names.size();
        if (cnt > 0) {
            _mapUILayer->createCardDeck(CardDeckType::Spell, (int)cnt);
            for (int i = 0; i < cnt; ++i) {
                _mapUILayer->insertCard(CardDeckType::Spell, names.at(i));
            }
        }
#endif
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
    for (int i = 0; i < _world->getFactionCount(); ++i) {
        updateUnits(_game, i);
    }
    
    // update bullets
    updateBullets(_game);
    
    // update ui layer
    updateUILayer();
}

void GameRender::updateUnits(const Game* game, int index)
{
    const Faction* f = _world->getFaction(index);
    
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
                node->update(game);
                if (_mapLayer) {
                    if (kSkillClass_Move == sc) {
                        _mapLayer->repositionUnit(node, pos);
                        if (_unitShadows.find(node) != end(_unitShadows)) {
                            const auto& pair = _unitShadows.at(node);
                            pair.first->setPosition(node->getPosition() + pair.second);
                        }
                    }
                    
                    _mapLayer->checkUnitWithSpellRing(node);
                }
            } else {
                if (kSkillClass_Die != sc) {
                    const int factionIndex = f->getFactionIndex();
                    // TODO: check if the unit is on the right
                    UnitNode* node = UnitNode::create(unit, factionIndex != 0);
                    node->registerObserver(this);
                    _mapLayer->addUnit(node, pos);
                    _allUnitNodes.insert(make_pair(key, node));
                    
                    if (factionIndex == game->getThisFactionIndex()) {
                        // add existent hero
                        const UnitType* unitType = unit->getUnitBase().getUnitType();
                        const UnitClass unitClass = unitType->getUnitClass();
                        if (kUnitClass_Building != unitClass && kUnitClass_Core != unitClass) {
                            _mapLayer->addUnitPlacementEffect(pos);
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
    for (int i = 0; i < _world->getBulletCount(); ++i) {
        auto bullet = _world->getBullet(i);
        const auto& pos = bullet->getPos();
        const auto& targetPos = bullet->targetPos();
        const bool isExploded(bullet->isExploded());
        if (_allBulletNodes.find(bullet) != _allBulletNodes.end()) {
            // already exist, update it
            auto node = _allBulletNodes.at(bullet);
            node->update();
            if (isExploded) {
                auto bt = bullet->getBulletType();
                if (bt && _mapLayer) {
                    _mapLayer->addBulletExplosionEffect(bt->getRenderKey(), pos);
                }
                node->removeFromParent();
                _allBulletNodes.erase(bullet);
                _bulletParams.erase(bullet);
            } else {
                const auto& params = _bulletParams.at(bullet);
                const auto& opos = params.first;
                const float h = params.second;
                const float d = sqrt(pow(abs(opos.x- targetPos.x), 2) + pow(abs(opos.y - targetPos.y), 2));
                const float distance = sqrt(pow(abs(pos.x- opos.x), 2) + pow(abs(pos.y - opos.y), 2));
                float height = 0;
                if (d > 0) {
                    const float bulletMaxHeightFactor = bullet->getBulletType()->getHeight() / 100.0f;
                    const float bulletMaxHeight = d * bulletMaxHeightFactor;
                    const float a = - (2.0f * bulletMaxHeight + h + 2.0f * sqrt(pow(bulletMaxHeight, 2) + h * bulletMaxHeight)) / pow(d, 2);
                    const float b = 2.0f * (bulletMaxHeight + sqrt(pow(bulletMaxHeight, 2) + h * bulletMaxHeight)) / d;
                    height = a * pow(distance, 2) + b * distance + h;
                    
                    const float beta = atan(2.0f * a * distance + b);
                    float scale = 1.0f;
                    if (targetPos.x == opos.x) {
                        node->setRotation(targetPos.y > opos.y ? -90 : 90);
                        scale = abs(cos(beta) + sin(beta) / eyeRadians);
                    } else {
                        const float alpha = atan(float(targetPos.y - opos.y) / (targetPos.x - opos.x));
                        const float gamma = atan(tan(beta)/cos(alpha)/eyeRadians + tan(alpha));
                        int32_t direction = targetPos.x < opos.x ? 1 : -1;
                        node->setRotation(CC_RADIANS_TO_DEGREES(gamma) * direction);
                        scale = abs(cos(alpha) * cos(beta) / cos(gamma));
                    }
                    node->setScale(scale * node->getScaleZ(), scale);
                    
                }
                _mapLayer->repositionUnit(node, pos + Coordinate32(0, height));
                
            }
        } else {
            if (!isExploded) {
                const auto distance = pos.distance(targetPos);
                const auto speed = bullet->getBulletType()->getSpeed();
                auto node = BulletNode::create(bullet, (float)distance / speed);
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
    
    if (_world) {
        for (auto iter = begin(_cores); iter != end(_cores); ++iter) {
            const Unit* core(iter->second);
            if (core) {
                const int maxHp = core->getUnitBase().getMaxHp();
                const int hp = core->getHp();
                const float percentage = 100 * (float)hp / (float)maxHp;
                if (iter->first == _game->getThisFactionIndex()) {
                    _mapUILayer->updateMyHpProgress(percentage);
                } else {
                    _mapUILayer->updateOpponentsHpProgress(percentage);
                }
            }
        }
    }
    
    _mapUILayer->updateRemainingTime(_remainingTime);
    
    if (_deck) {
//        const int counter = _deck->getCounter();
//        const int total = _deck->getDrawSpanFrames();
//        const float duration = (float)total / (float)GameConstants::FRAME_PER_SEC;
//        const float time = (1.0 - (float)(counter % total) / (float)total) * duration;
//        _mapUILayer->updateCountDown(time, duration);
//        
//        const auto& logs = _deck->getLogs();
//        for (int i = 0; i < logs.size(); ++i) {
//            const auto& log = logs.at(i);
//            const auto event = log._event;
//            const auto card = log._card;
//            if (Deck::kDeckEvent_Use == event) {
//                _mapUILayer->removeCard(CardDeckType::Unit, card, log._extra);
//            } else if (Deck::kDeckEvent_Draw == event) {
//                _mapUILayer->insertCard(CardDeckType::Unit, card);
//            }
//        }
//        
        _mapUILayer->updateNextCard(_deck->getNextDraw());
        
        // TODO:
        bool needReload(false);
        const int cnt = _deck->getHandCount();
        if (cnt != _handCards.size()) {
            needReload = true;
        } else {
            for (int i = 0; i < cnt; ++i) {
                if (_deck->getHandCard(i)->getCardType()->getName() != _handCards.at(i)) {
                    needReload = true;
                    break;
                }
            }
        }
        
        if (needReload) {
            static const auto type(CardDeckType::Unit);
            _mapUILayer->clear(type);
            _handCards.clear();
            for (int i = 0; i < cnt; ++i) {
                auto card = _deck->getHandCard(i);
                if (card) {
                    const string& name = card->getCardType()->getName();
                    _mapUILayer->insertCard(type, card /*name*/, false);
                    _handCards.push_back(name);
                }
            }
        }
//        _deck->clearEventLog();
    }
    
    auto core = getCore();
    const auto& names = getSpells();
    for (int i = 0; i < names.size(); ++i) {
        auto spell = core->getSpellByIndex(i);
        if (spell) {
            const float percentage = (float)spell->getCDProgress() / (float)spell->getTotalCDFrames();
            _mapUILayer->updateCardCD(CardDeckType::Spell, i, percentage);
        }
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
void GameRender::onUnitNodeCreateShadow(UnitNode* node, Node* shadow, const Point& offset)
{
    if (node && shadow) {
        auto parent = node->getParent();
        shadow->setPosition(node->getPosition() + offset);
        parent->addChild(shadow, -2000);
        CCASSERT(_unitShadows.find(node) == end(_unitShadows), "shadow has exist.");
        _unitShadows.insert(make_pair(node, make_pair(shadow, offset)));
    }
}

void GameRender::onUnitNodeRemoveShadow(UnitNode* node)
{
    if (node && _unitShadows.find(node) != end(_unitShadows)) {
        auto shadow = _unitShadows.at(node).first;
        shadow->stopAllActions();
        shadow->removeFromParent();
        _unitShadows.erase(node);
    }
}

void GameRender::onUnitNodePlayedDeadAnimation(int unitId)
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
    updateCardMask(_selectedCard.first, point);
}

void GameRender::onMapLayerTouchMoved(const Point& point, bool isValid)
{
    if (isValid) {
        updateCardMask(_selectedCard.first, point);
    } else {
        removeCardMask();
    }
}

void GameRender::onMapLayerTouchEnded(const Point& point)
{
    const string& card = _selectedCard.first;
    updateCardMask(card, point);
    tryToUseCard(card, _selectedCard.second, point);
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

void GameRender::onMapUILayerCardSelected(const string& card, int idx)
{
    _selectedCard.first = card;
    _selectedCard.second = idx;
    
    if (_mapLayer) {
        if (card.length() > 0) {
            const HMMCardType* ct = getCardType(card);
            if (ct && kHMMCardClass_Spell != ct->getCardClass()) {
                _mapLayer->setPlacedArea(_minPuttingX, _maxPuttingX);
            }
        } else {
            _mapLayer->setPlacedArea(INVALID_VALUE, INVALID_VALUE);
        }
    }
}

void GameRender::onMapUILayerTouchMoved(const string& card, const Point& point)
{
    updateCardMask(card, convertToMapLayer(point));
}

void GameRender::onMapUILayerTouchEnded(const string& card, int idx, const Point& point)
{
    const Point& p = convertToMapLayer(point);
    updateCardMask(card, p);
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
        auto node = _allUnitNodes.at(unitId);
        node->removeFromParent();
        _allUnitNodes.erase(unitId);
        
        onUnitNodeRemoveShadow(node);
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
    
    for (auto iter = begin(_unitShadows); iter != end(_unitShadows); ++iter) {
        iter->second.first->removeFromParent();
    }
    
    _allUnitNodes.clear();
    _unitShadows.clear();
    _cores.clear();
}

void GameRender::pauseGame()
{
    MessageBoxLayer::getInstance()->show(LocalHelper::getString("hint_exitPve"), MessageBoxType::YesNo, [](Ref*) {
        CocosUtils::replaceScene(MainLayer::createScene(), true);
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
    if (_world) {
        const Faction* faction = _world->getFaction(_game->getThisFactionIndex());
        const int count = _techTree->getResourceTypeCount();
        for (int i = 0; i < count; ++i) {
            const UnderWorld::Core::ResourceType* resourceType = _techTree->getResourceTypeByIndex(i);
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
    const Unit* myCore = _cores.at(_game->getThisFactionIndex());
    if (myCore && myCore->isAlive()) {
        win = true;
    }
    
    for (auto iter = begin(_allUnitNodes); iter != end(_allUnitNodes); ++iter) {
        UnitNode* node = iter->second;
        const int factionIndex = node->getUnit()->getBelongFaction()->getFactionIndex();
        if (factionIndex == _game->getThisFactionIndex()) {
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

void GameRender::updateCardMask(const string& card, const Point& point)
{
    if (_mapLayer && _mapUILayer && card.length() > 0) {
        const bool inDeck = _mapUILayer->isPointInDeck(convertToUILayer(point));
        if (inDeck) {
            removeCardMask();
        } else {
            auto ct = getCardType(card);
            if (ct) {
                auto type = ct->getCardClass();
                if (kHMMCardClass_Summon == type) {
                    auto ut = _techTree->findUnitTypeByName(ct->getCustomUnitSetting().getUnitTypeName());
                    if (ut) {
                        auto coordinate = getValidPuttingCoordinate(point, true);
                        _mapLayer->updateUnitMask(ut, coordinate);
                    }
                } else if (kHMMCardClass_Spell == type) {
                    const auto& name(ct->getSpellName());
                    auto st = getSpellType(name);
                    if (isValidAoeSpell(st)) {
                        auto coordinate = getValidPuttingCoordinate(point, false);
                        _mapLayer->updateSpellRing(name, coordinate, st->getSpellRadius());
                    }
                }
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

void GameRender::tryToUseCard(const string& card, int idx, const Point& point)
{
    if (_mapLayer && card.length() > 0) {
        const bool inDeck = _mapUILayer->isPointInDeck(convertToUILayer(point));
        if (inDeck) {
            // TODO:
        } else if (_commander && _deck) {
            auto ct = getCardType(card);
            auto type = ct->getCardClass();
            auto coordinate = getValidPuttingCoordinate(point, kHMMCardClass_Summon == type);
            auto result = _commander->addCommandFromLocal(new OutsideHMMCommand(idx, _deck->getFactionIndex(), coordinate));
            
            if (kCommandResult_suc == result) {
                if (kHMMCardClass_Spell == type) {
                    const auto spellName = ct->getSpellName();
                    const bool isAOE(isValidAoeSpell(getSpellType(spellName)));
                    if (isAOE) {
                        if (spellName.find(SPELL_NAME_FIREBALL) != string::npos) {
                            auto core = getCore();
                            if (core) {
                                const int unitId = core->getUnitId();
                                if (_allUnitNodes.find(unitId) != end(_allUnitNodes)) {
                                    auto node = _allUnitNodes.at(unitId);
                                    if (node) {
                                        _mapLayer->addAoeSpell(node->getPosition(), spellName, 2.0f);
                                    }
                                }
                            }
                        } else if (spellName.find(SPELL_NAME_CURE) != string::npos ||
                                   spellName.find(SPELL_NAME_SPEEDUP) != string::npos) {
                            _mapLayer->addSpell(spellName, 12.0f);
                        } else {
                            _mapLayer->addSpell(spellName, 1.0f);
                        }
                    }
                }
                
                if (_mapUILayer) {
                    _mapUILayer->clearHighlightedCard();
                }
            }
        }
        
        _mapLayer->removeSpellRing();
        _mapLayer->removeUnitMask();
    }
}

const Unit* GameRender::getCore() const
{
    const int fi = _game->getThisFactionIndex();
    if (_cores.find(fi) != end(_cores)) {
        return _cores.at(fi);
    }
    
    return nullptr;
}

const vector<string>& GameRender::getSpells() const
{
    const auto core = getCore();
    if (core) {
        auto ut = core->getUnitBase().getUnitType();
        if (ut) {
            return vector<string>();
            //return ut->getSpellNames();
        }
    }
    
    static auto v = vector<string>();
    return v;
}

const HMMCardType* GameRender::getCardType(const string& name) const
{
    if (_gameModeHMM) {
        return _gameModeHMM->findCardTypeByName(name);
    }
    
    return nullptr;
}

const SpellType* GameRender::getSpellType(const string& name) const
{
    if (_techTree) {
        return _techTree->findSpellTypeByName(name);
    }
    
    return nullptr;
}

Coordinate32 GameRender::getValidPuttingCoordinate(const Point& point, bool check) const
{
    if (_mapLayer) {
        Coordinate32 coordinate = _mapLayer->point2Coordinate(point);
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
