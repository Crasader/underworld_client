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
#include "UnitNode.h"
#include "BulletNode.h"
#include "MapUILayer.h"

USING_NS_CC;

class MapLayer;

namespace UnderWorld { namespace Core {
    class Game;
    class Commander;
}}

class GameRenderObserver
{
public:
    virtual ~GameRenderObserver() {}
};

class GameRender : public UnderWorld::Core::AbstractRender, public UnitNodeObserver, public BulletNodeObserver, public MapUILayerObserver
{
public:
    GameRender(Node* scene, int mapId, const std::string& opponentsAccount);
    virtual ~GameRender();
    void registerObserver(GameRenderObserver *observer);
    
    virtual void init(const UnderWorld::Core::Game* game, UnderWorld::Core::Commander* commander) override;
    virtual void render(const UnderWorld::Core::Game* game) override;
    
    MapLayer* getMapLayer() const;
    MapUILayer* getMapUILayer() const;
    
protected:
    // UnitNodeObserver
    virtual void onUnitNodePlayDeadAnimationFinished(UnitNode* node) override;
    
    // BulletNodeObserver
    virtual void onBulletNodeReachedTarget(BulletNode* node) override;
    
    // MapUILayerObserver
    virtual void onMapUILayerUnitSelected(MapUIUnitNode* node) override;
    virtual void onMapUILayerClickedPauseButton(bool pause) override;
    
private:
    void updateUnits(const UnderWorld::Core::Game* game, int index);
    void updateBullets(const UnderWorld::Core::Game* game);
    
private:
    GameRenderObserver *_observer;
    MapLayer* _mapLayer;
    MapUILayer* _mapUILayer;
    const UnderWorld::Core::Game* _game;
    UnderWorld::Core::Commander* _commander;
    std::map<int, UnitNode*> _allUnits;
    std::map<int64_t, BulletNode*> _allBullets;
};

#endif /* GameRender_h */
