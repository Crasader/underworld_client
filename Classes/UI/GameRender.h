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
#include "VictoryLayer.h"

USING_NS_CC;

class MapLayer;

namespace UnderWorld { namespace Core {
    class Game;
    class Commander;
    class Unit;
}}

class GameRenderObserver
{
public:
    virtual ~GameRenderObserver() {}
    virtual void onGameRenderRestart() = 0;
};

class GameRender : public UnderWorld::Core::AbstractRender, public UnitNodeObserver, public BulletNodeObserver, public MapUILayerObserver, public VictoryLayerObserver
{
public:
    GameRender(Scene* scene, int mapId, MapLayer* mapLayer, const std::string& opponentsAccount);
    virtual ~GameRender();
    void registerObserver(GameRenderObserver *observer);
    
    virtual void init(const UnderWorld::Core::Game* game, UnderWorld::Core::Commander* commander) override;
    virtual void render(const UnderWorld::Core::Game* game) override;
    
protected:
    // UnitNodeObserver
    virtual void onUnitNodePlayDeadAnimationFinished(UnitNode* node) override;
    virtual void onUnitNodeHurtTheTarget(UnitNode* node) override;
    
    // BulletNodeObserver
    virtual void onBulletNodeReachedTarget(BulletNode* node) override;
    virtual void onBulletNodeExploded(BulletNode* node) override;
    
    // MapUILayerObserver
    virtual void onMapUILayerUnitSelected(MapUIUnitNode* node) override;
    virtual void onMapUILayerClickedPauseButton() override;
    virtual ssize_t onMapUILayerCampsCount() override;
    virtual const UnderWorld::Core::Camp* onMapUILayerCampAtIndex(ssize_t idx) override;
    
    // VictoryLayerObserver
    virtual void onVictoryLayerClosed(Layer* pSender) override;
    virtual void onVictoryLayerContinued(Layer* pSender) override;
    
private:
    void updateUnits(const UnderWorld::Core::Game* game, int index);
    void updateBullets(const UnderWorld::Core::Game* game);
    void updateUILayer();
    void hurtUnit(const UnderWorld::Core::Unit* target, const std::string& trigger);
    void removeAllBullets();
    void removeAllUnits();
    void pauseGame();
    void resumeGame();
    void restartGame();
    void tick(float dt);
    void updateResources();
    void initUILayer();
    void stopAllTimers();
    void onGameOver();
    
private:
    GameRenderObserver *_observer;
    Scene* _scene;
    int _mapId;
    MapLayer* _mapLayer;
    MapUILayer* _mapUILayer;
    const UnderWorld::Core::Game* _game;
    UnderWorld::Core::Commander* _commander;
    std::map<int, UnitNode*> _allUnits;
    std::map<int64_t, BulletNode*> _allBullets;
    std::map<int, UnderWorld::Core::Unit*> _cores;
    bool _paused;
    bool _isGameOver;
    int _waveTime;
    int _remainingTime;
};

#endif /* GameRender_h */
