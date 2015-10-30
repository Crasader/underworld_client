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
    
}

void GameRender::init(const Game* game)
{
    // create units
    updateUnits(game, 0);
    updateUnits(game, 1);
    
    // create bullets
    updateBullets(game);
}

void GameRender::render(const Game* game)
{
    
}

void GameRender::updateUnits(const Game* game, int index)
{
    const World* world = game->getWorld();
    const std::vector<Faction*>& factions = world->getFactions();
    if (factions.size() > index) {
        const Faction* f = factions.at(index);
        const std::vector<Unit*>& units = f->getAllUnits();
        for (int i = 0; i < units.size(); ++i) {
            const Unit* unit = units.at(i);
            const Coordinate& pos = unit->getPos();
            const int key = unit->getUnitId();
            if (_allUnits.find(key) != _allUnits.end()) {
                // already exist, update it
                UnitNode* node = _allUnits.at(key);
                node->update();
                const Skill* skill = unit->getCurrentSkill();
                if (skill->getSkillState() == Skill::kSkillState_performing) {
                    SkillClass sc = skill->getSkillType()->getSkillClass();
                    if (kSkillClass_Move == sc) {
                        _mapLayer->repositionUnit(node, pos);
                    }
                }
                
            } else {
                UnitNode* node = UnitNode::create(unit);
                _mapLayer->addUnit(node, pos);
                _allUnits.insert(make_pair(key, node));
            }
        }
    }
}

void GameRender::updateBullets(const Game* game)
{
    const World* world = game->getWorld();
    const std::vector<Bullet*>& bullets = world->getBullets();
    for (int i = 0; i < bullets.size(); ++i) {
        const Bullet* bullet = bullets.at(i);
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