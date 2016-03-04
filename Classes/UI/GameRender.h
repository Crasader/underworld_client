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
#include "MapLayer.h"
#include "MapUILayer.h"
#include "VictoryLayer.h"
#include "DefeatLayer.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class Game;
    class Commander;
    class Unit;
    class Deck;
    class Card;
}}

class GameRenderObserver
{
public:
    virtual ~GameRenderObserver() {}
    virtual void onGameRenderRestart() = 0;
};

class GameRender
: public UnderWorld::Core::AbstractRender
, public UnitNodeObserver
, public BulletNodeObserver
, public MapLayerObserver
, public MapUILayerObserver
, public VictoryLayerObserver
, public DefeatLayerObserver
{
public:
    GameRender(Scene* scene, int mapId, const std::string& mapData, const std::string& opponentsAccount);
    virtual ~GameRender();
    void registerObserver(GameRenderObserver *observer);
    const UnderWorld::Core::MapSetting& getMapSetting() const;
    
    virtual void init(const UnderWorld::Core::Game* game, UnderWorld::Core::Commander* commander) override;
    virtual void render(const UnderWorld::Core::Game* game) override;
    
protected:
    // UnitNodeObserver
    virtual void onUnitNodeUpdatedFeatures(int unitId) override;
    virtual void onUnitNodePlayDeadAnimationFinished(int unitId) override;
    virtual void onUnitNodeHurtTheTarget(UnitNode* node) override;
    virtual void onUnitNodeShakeScreen(UnitNode* node) override;
    
    // BulletNodeObserver
    virtual void onBulletNodeReachedTarget(BulletNode* node) override;
    virtual void onBulletNodeExploded(BulletNode* node) override;
    
    // MapLayerObserver
    virtual void onMapLayerTouchEnded(const Point& point) override;
    
    // MapUILayerObserver
    virtual bool onMapUILayerIsGameOver() const override;
    virtual void onMapUILayerClickedPauseButton() override;
    virtual void onMapUILayerCardSelected(const UnderWorld::Core::Card* card, int idx) override;
    virtual void onMapUILayerTouchMoved(const UnderWorld::Core::Card* card, const Point& point, bool inDeck) override;
    virtual void onMapUILayerTouchEnded(const UnderWorld::Core::Card* card, int idx, const Point& position) override;
    
    // VictoryLayerObserver
    virtual void onVictoryLayerClosed(Layer* pSender) override;
    virtual void onVictoryLayerContinued(Layer* pSender) override;
    
    // DefeatLayerObserver
    virtual void onDefeatLayerClosed(Layer* pSender) override;
    virtual void onDefeatLayerContinued(Layer* pSender) override;
    
private:
    void updateAll();
    void updateUnits(const UnderWorld::Core::Game* game, int index);
    void updateBullets(const UnderWorld::Core::Game* game);
    void updateUILayer();
    bool isValidAoeSpell(const UnderWorld::Core::SpellType* spellType) const;
    void hurtUnit(const UnderWorld::Core::Unit* target, const std::string& trigger);
    void removeUnit(int unitId);
    void removeAllBullets();
    void removeAllUnits();
    void pauseGame();
    void resumeGame();
    void restartGame();
    void tick(float dt);
    void updateResources();
    void stopAllTimers();
    void onGameOver();
    
    Point convertToMapLayer(const Point& uiLayerPoint) const;
    Point convertToUILayer(const Point& mapLayerPoint) const;
    
private:
    GameRenderObserver *_observer;
    Scene* _scene;
    int _mapId;
    MapLayer* _mapLayer;
    MapUILayer* _mapUILayer;
    const UnderWorld::Core::Game* _game;
    UnderWorld::Core::Commander* _commander;
    std::map<int, UnitNode*> _allUnitNodes;
    std::map<int64_t, std::pair<UnderWorld::Core::Coordinate, float>> _bulletParams;
    std::map<int64_t, BulletNode*> _allBulletNodes;
    std::map<int, const UnderWorld::Core::Unit*> _cores;
    const UnderWorld::Core::Deck* _deck;
    std::pair<const UnderWorld::Core::Card* ,int> _selectedCard;
    
    bool _paused;
    bool _isGameOver;
    int _remainingTime;
    std::map<std::string, float> _resources;
};

#endif /* GameRender_h */
