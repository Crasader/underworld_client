//
//  GameRender.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/30.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GameRender.h"
#include "Game.h"
#include "MapLayer.h"
#include "UnitNode.h"
#include "BulletNode.h"
#include "Map.h"
#include "Camp.h"

using namespace UnderWorld::Core;

GameRender::GameRender(Node* scene, int mapId, const std::string& opponentsAccount)
:_observer(nullptr)
,_mapLayer(nullptr)
,_mapUILayer(nullptr)
,_game(nullptr)
,_commander(nullptr)
{
    assert(scene);
    _mapLayer = MapLayer::create(mapId);
    scene->addChild(_mapLayer);
    
    _mapUILayer = MapUILayer::create("我的名字", opponentsAccount);
    _mapUILayer->registerObserver(this);
    scene->addChild(_mapUILayer);
}

GameRender::~GameRender()
{
    for (int i = 0; i < _allUnits.size(); ++i) {
        _allUnits.at(i)->removeFromParent();
    }
    
    for (int i = 0; i < _allBullets.size(); ++i) {
        _allBullets.at(i)->removeFromParent();
    }
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
    
    if (_mapUILayer) {
        _mapUILayer->initWithGame(game);
    }
}

void GameRender::render(const Game* game)
{
    assert(game = _game);
    
    // create units
    for (int i = 0; i < game->getWorld()->getFactionCount(); ++i) {
        updateUnits(game, i);
        
    }
    
    // create bullets
    updateBullets(game);
}

void GameRender::updateUnits(const Game* game, int index)
{
    const World* world = game->getWorld();

    const Faction* f = world->getFaction(index);
    const std::vector<Unit*>& units = f->getAllUnits();
    for (int i = 0; i < units.size(); ++i) {
        const Unit* unit = units.at(i);
        const Coordinate& pos = unit->getPos();
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

void GameRender::addCritEffect(const Unit* target, const string& trigger)
{
    if (target && kSkillClass_Die != target->getCurrentSkill()->getSkillType()->getSkillClass()) {
        const int key = target->getUnitId();
        if (_allUnits.find(key) != _allUnits.end()) {
            UnitNode* node = _allUnits.at(key);
            if (node) {
                node->addCritEffect(trigger);
            }
        }
    }
}

MapLayer* GameRender::getMapLayer() const
{
    return _mapLayer;
}

MapUILayer* GameRender::getMapUILayer() const
{
    return _mapUILayer;
}

#pragma mark - UnitNodeObserver
void GameRender::onUnitNodePlayDeadAnimationFinished(UnitNode* node)
{
    node->removeFromParent();
    _allUnits.erase(node->getUnit()->getUnitId());
}

void GameRender::onUnitNodeFootmanAttackedTheTarget(UnitNode* node)
{
    const Unit* unit = node->getUnit();
    Unit* target = unit->getTarget();
    addCritEffect(target, unit->getUnitType()->getName());
    node->addSwordEffect();
}

#pragma mark - BulletNodeObserver
void GameRender::onBulletNodeReachedTarget(BulletNode* node)
{
    
}

void GameRender::onBulletNodeExploded(BulletNode* node)
{
    Unit* target = node->getBullet()->getTarget();
    const Unit* trigger = node->getBullet()->getTrigger();
    addCritEffect(target, trigger->getUnitType()->getName());
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
                    CCLOG("========== Add command ==========");
                    
                    // TODO: replace the temp code with callback from camp
                    Scheduler *s = Director::getInstance()->getScheduler();
                    if (s) {
                        static string key("temp_test_update_MapUIUnitNode");
                        s->schedule([=](float dt) {
                            s->unschedule(key, this);
                            CCLOG("========== Update UI ==========");
                            node->update(false);
                        }, this, 0.1f, false, key);
                    }
                }
            }
        }
    }
}

void GameRender::onMapUILayerClickedPauseButton(bool pause)
{
    
}