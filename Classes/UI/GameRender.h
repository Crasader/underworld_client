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

// UnderWorld::Core
namespace UnderWorld { namespace Core {
    class Game;
    class World;
    class TechTree;
    class GameModeHMM;
    class Commander;
    class Unit;
    class UnitType;
    class HMMCardType;
    class SpellType;
    class HMMDeck;
}}

USING_NS_CC;
using namespace UnderWorld::Core;

class GameRenderObserver
{
public:
    virtual ~GameRenderObserver() {}
    virtual void onGameRenderRestart() = 0;
    virtual void onGameRenderExit() = 0;
};

class GameRender
: public AbstractRender
, public UnitNodeObserver
, public BulletNodeObserver
, public MapLayerObserver
, public MapUILayerObserver
, public VictoryLayerObserver
, public DefeatLayerObserver
{
public:
    GameRender(Scene* scene, const std::string& opponentsAccount);
    virtual ~GameRender();
    void registerObserver(GameRenderObserver *observer);
    
    const Size& getMapSize() const;
    Coordinate32 point2Coordinate(const Point& point) const;
    
protected:
    // AbstractRender
    virtual void init(const Game* game, Commander* commander) override;
    virtual void render(const Game* game) override;
    
    // UnitNodeObserver
    virtual void onUnitNodeCreateShadow(UnitNode* node, Node* shadow, const Point& offset) override;
    virtual void onUnitNodeRemoveShadow(UnitNode* node) override;
    virtual void onUnitNodePlayedDeadAnimation(int unitId) override;
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
    virtual void onMapUILayerCardSelected(int card, int idx) override;
    virtual void onMapUILayerTouchMoved(int card, const Point& point) override;
    virtual void onMapUILayerTouchEnded(int card, int idx, const Point& position) override;
    
    // VictoryLayerObserver
    virtual void onVictoryLayerClosed(Layer* pSender) override;
    virtual void onVictoryLayerContinued(Layer* pSender) override;
    
    // DefeatLayerObserver
    virtual void onDefeatLayerClosed(Layer* pSender) override;
    virtual void onDefeatLayerContinued(Layer* pSender) override;
    
private:
    void updateAll();
    void updateUnits(const Game* game, int index);
    void updateBullets(const Game* game);
    void updateUILayer();
    bool hasEnoughResources(const HMMCardType* ct) const;
    bool isValidAoeSpell(const SpellType* spellType) const;
    void hurtUnit(const Unit* target, const std::string& trigger);
    void removeUnit(int unitId);
    void removeAllBullets();
    void removeAllUnits();
    void pauseGame();
    void resumeGame();
    void tick(float dt);
    void updateResources();
    void stopAllTimers();
    void onGameOver();
    
    Point convertToMapLayer(const Point& uiLayerPoint) const;
    Point convertToUILayer(const Point& mapLayerPoint) const;
    
    //
    void updateCardMask(int card, const Point& point);
    void removeCardMask();
    void tryToUseCard(int card, int idx, const Point& point);
    
    const Unit* getCore() const;
    const HMMCardType* getCardType(int card) const;
    const SpellType* getSpellType(int card) const;
    Coordinate32 getValidPuttingCoordinate(const Point& point, bool check) const;
    
private:
    GameRenderObserver *_observer;
    Scene* _scene;
    int _mapId;
    std::string _opponentsAccount;
    MapLayer* _mapLayer;
    MapUILayer* _mapUILayer;
    
    const Game* _game;
    const World* _world;
    const TechTree* _techTree;
    const GameModeHMM* _gameModeHMM;
    Commander* _commander;
    std::unordered_map<int, UnitNode*> _allUnitNodes;
    std::unordered_map<UnitNode*, std::set<std::pair<Node*, Point>>> _unitShadows;
    std::unordered_map<const void*, std::pair<Coordinate32, float>> _bulletParams;
    std::unordered_map<const void*, BulletNode*> _allBulletNodes;
    std::unordered_map<int, const Unit*> _cores;
    const HMMDeck* _deck;
    std::vector<int> _handCards;
    std::pair<int ,int> _selectedCard;
    
    bool _paused;
    bool _isGameOver;
    int _remainingTime;
    std::unordered_map<std::string, float> _resources;
    
    int _minPuttingX;
    int _maxPuttingX;
};

#endif /* GameRender_h */
