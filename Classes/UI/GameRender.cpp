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

using namespace UnderWorld::Core;

GameRender::GameRender(Node* scene, int mapId)
:_mapLayer(nullptr)
{
    assert(scene);
    _mapLayer = MapLayer::create(mapId);
    scene->addChild(_mapLayer);
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
    // create units
    for (int i = 0; game->getWorld()->getFactionCount(); ++i) {
        updateUnits(game, i);

    }
    
    // create bullets
    updateBullets(game);
}

void GameRender::render(const Game* game)
{
    // create units
    for (int i = 0; game->getWorld()->getFactionCount(); ++i) {
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
        if (true || skill->getSkillState() == Skill::kSkillState_performing) {
            SkillClass sc = skill->getSkillType()->getSkillClass();
            bool isNewCreated = (_allUnits.find(key) == _allUnits.end()) ? true : false;
            if (isNewCreated && kSkillClass_Die != sc) {
                UnitNode* node = UnitNode::create(unit);
                _mapLayer->addUnit(node, pos);
                _allUnits.insert(make_pair(key, node));
            } else if (!isNewCreated) {
                // already exist, update it
                UnitNode* node = _allUnits.at(key);
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
                    {
                        node->removeFromParent();
                        _allUnits.erase(key);
                    }
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
        if (_allBullets.find(key) != _allBullets.end()) {
            // already exist, update it
            BulletNode* node = _allBullets.at(key);
            node->update();
            _mapLayer->repositionUnit(node, pos);
        } else {
            BulletNode* node = BulletNode::create(bullet);
            _mapLayer->addUnit(node, pos);
            _allBullets.insert(make_pair(key, node));
        }
    }
}

MapLayer* GameRender::getMapLayer()
{
    return _mapLayer;
}