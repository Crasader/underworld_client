//
//  GameRender.h
//  Underworld_Client
//
//  Created by Andy on 15/10/30.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GameRender_h
#define GameRender_h

#include "cocos2d.h"
#include "GameInterface.h"

USING_NS_CC;

class MapLayer;
class UnitNode;
class BulletNode;

class GameRender : public UnderWorld::Core::AbstractRender
{
public:
    GameRender(Node* scene, int mapId);
    virtual ~GameRender();
    
    virtual void init(const UnderWorld::Core::Game* game) override;
    virtual void render(const UnderWorld::Core::Game* game) override;
    
    MapLayer* getMapLayer();
    
private:
    void updateUnits(const UnderWorld::Core::Game* game, int index);
    void updateBullets(const UnderWorld::Core::Game* game);
    
private:
    MapLayer* _mapLayer;
    std::map<int, UnitNode*> _allUnits;
    std::map<int, BulletNode*> _allBullets;
};

#endif /* GameRender_h */
