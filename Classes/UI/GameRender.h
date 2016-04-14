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
#include "ExternalInterface.h"
#include "UnitNode.h"
#include "BulletNode.h"
#include "MapLayer.h"
#include "MapUILayer.h"
#include "VictoryLayer.h"
#include "DefeatLayer.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class Game;
    class TechTree;
    class Commander;
    class Unit;
    class UnitType;
    class Deck;
    class CardType;
    class SpellType;
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
    
protected:
    // AbstractRender
    virtual void init(const UnderWorld::Core::Game* game, UnderWorld::Core::Commander* commander) override;
    virtual void render(const UnderWorld::Core::Game* game) override;
    
    // UnitNodeObserver
    virtual void onUnitNodePlayDeadAnimationFinished(int unitId) override;
    virtual void onUnitNodeHurtTheTarget(UnitNode* node) override;
    virtual void onUnitNodeShakeScreen(UnitNode* node) override;
    
    // BulletNodeObserver
    virtual void onBulletNodeReachedTarget(BulletNode* node) override;
    virtual void onBulletNodeExploded(BulletNode* node) override;
    
    // MapLayerObserver
    virtual void onMapLayerTouchBegan(const Point& point) override;
    virtual void onMapLayerTouchMoved(const Point& point, bool isValid) override;
    virtual void onMapLayerTouchEnded(const Point& point) override;
    
    // MapUILayerObserver
    virtual bool onMapUILayerIsGameOver() const override;
    virtual void onMapUILayerClickedPauseButton() override;
    virtual void onMapUILayerCardSelected(const std::string& card, int idx) override;
    virtual void onMapUILayerTouchMoved(const std::string& card, const Point& point) override;
    virtual void onMapUILayerTouchEnded(const std::string& card, int idx, const Point& position) override;
    
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
    
    //
    void updateCardMask(const std::string& card, const Point& point, float range);
    void removeCardMask();
    void tryToUseCard(const std::string& card, int idx, const Point& point);
    
    const UnderWorld::Core::TechTree* getTechTree() const;
    const UnderWorld::Core::CardType* getCardType(const std::string& name) const;
    const UnderWorld::Core::UnitType* getUnitType(const std::string& name) const;
    const UnderWorld::Core::SpellType* getSpellType(const std::string& name) const;
    UnderWorld::Core::Coordinate32 getValidPuttingCoordinate(const Point& point, bool check) const;
    
private:
    GameRenderObserver *_observer;
    Scene* _scene;
    int _mapId;
    MapLayer* _mapLayer;
    MapUILayer* _mapUILayer;
    const UnderWorld::Core::Game* _game;
    UnderWorld::Core::Commander* _commander;
    std::unordered_map<int, UnitNode*> _allUnitNodes;
    std::unordered_map<const void*, std::pair<UnderWorld::Core::Coordinate32, float>> _bulletParams;
    std::unordered_map<const void*, BulletNode*> _allBulletNodes;
    std::unordered_map<int, const UnderWorld::Core::Unit*> _cores;
    const UnderWorld::Core::Deck* _deck;
    std::pair<std::string ,int> _selectedCard;
    
    bool _paused;
    bool _isGameOver;
    int _remainingTime;
    std::unordered_map<std::string, float> _resources;
    
    int _minPuttingX;
    int _maxPuttingX;
};

#endif /* GameRender_h */
