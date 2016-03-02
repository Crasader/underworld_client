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
    class Camp;
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
    virtual void onMapLayerTouchMoved(const Point& point) override;
    virtual void onMapLayerTouchEnded(const Point& point) override;
    
    // MapUILayerObserver
    virtual bool onMapUILayerIsGameOver() const override;
    virtual void onMapUILayerClickedPauseButton() override;
    virtual bool onMapUILayerIsHeroAlive(const UnderWorld::Core::Camp* camp) const override;
    virtual void onMapUILayerUnitSelected(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUILayerUnitTouched(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUILayerTouchMoved(const UnderWorld::Core::Camp* camp, const Point& point) override;
    virtual void onMapUILayerTouchCancelled(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUILayerTouchEnded(const UnderWorld::Core::Camp* camp, const Point& point) override;
    
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
    bool isCampFull(const UnderWorld::Core::Camp* camp) const;
    bool isValidAoeSpell(const UnderWorld::Core::Spell* spell) const;
    bool isProducibleCamp(const UnderWorld::Core::Camp* camp) const;
    const UnderWorld::Core::Spell* getSpell(const UnderWorld::Core::Camp* camp, int idx) const;
    UnderWorld::Core::CommandResult castSpell(const UnderWorld::Core::Spell* spell, const UnderWorld::Core::Unit* trigger, const UnderWorld::Core::Coordinate& coordinate, const UnderWorld::Core::Unit* target);
    UnitNode* getHeroUnitNode(const UnderWorld::Core::Camp* camp);
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
    
    // random
    const UnderWorld::Core::Camp* generateRandomCamp() const;
    
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
    std::vector<const UnderWorld::Core::Camp*> _myCamps;
    std::map<int, const UnderWorld::Core::Unit*> _cores;
    const UnderWorld::Core::Camp* _selectedCamp;
    std::set<const UnderWorld::Core::Camp*> _pickedCamps;
    
    bool _paused;
    bool _isGameOver;
    int _remainingTime;
    int _cardDeckTime;
    float _goldCount;
    float _woodCount;
    bool _hasUpdatedBattleCampInfos;
};

#endif /* GameRender_h */
