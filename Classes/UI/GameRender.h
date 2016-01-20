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
    class Camp;
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
    virtual void onUnitNodeUpdatedFeatures(int unitId) override;
    virtual void onUnitNodePlayDeadAnimationFinished(int unitId) override;
    virtual void onUnitNodeHurtTheTarget(UnitNode* node) override;
    
    // BulletNodeObserver
    virtual void onBulletNodeReachedTarget(BulletNode* node) override;
    virtual void onBulletNodeExploded(BulletNode* node) override;
    
    // MapUILayerObserver
    virtual void onMapUILayerUnitSelected(MapUIUnitNode* node) override;
    virtual void onMapUILayerClickedPauseButton() override;
    virtual ssize_t onMapUILayerCampsCount() override;
    virtual const UnderWorld::Core::Camp* onMapUILayerCampAtIndex(ssize_t idx) override;
    virtual void onMapUILayerSpellRingMoved(const UnderWorld::Core::Camp* camp, const Point& position) override;
    virtual void onMapUILayerSpellRingCancelled() override;
    virtual void onMapUILayerTryToCastSpell(const UnderWorld::Core::Camp* camp, const Point& position) override;
    
    // VictoryLayerObserver
    virtual void onVictoryLayerClosed(Layer* pSender) override;
    virtual void onVictoryLayerContinued(Layer* pSender) override;
    
private:
    void updateUnits(const UnderWorld::Core::Game* game, int index);
    void updateBullets(const UnderWorld::Core::Game* game);
    void updateUILayer();
    void updateBattleCampInfos();
    bool isCampFull(const UnderWorld::Core::Camp* camp) const;
    UnderWorld::Core::Spell* getSpell(const UnderWorld::Core::Camp* camp, int idx, UnderWorld::Core::Unit** trigger) const;
    UnderWorld::Core::SpellCastType getSpellCastType(const UnderWorld::Core::Camp* camp, int idx) const;
    bool canCastSpell(const UnderWorld::Core::Camp* camp, std::string& spellAlias, UnderWorld::Core::Unit** trigger) const;
    void castSpell(const UnderWorld::Core::Camp* camp, const UnderWorld::Core::Coordinate& coordinate, UnderWorld::Core::Unit* target);
    void hurtUnit(const UnderWorld::Core::Unit* target, const std::string& trigger);
    void removeUnit(int unitId);
    void removeAllBullets();
    void removeAllUnits();
    void removeBattleCampInfos();
    void pauseGame();
    void resumeGame();
    void restartGame();
    void tick(float dt);
    void updateResources();
    void initUILayer();
    int getRemainingWaveTime() const;
    void stopAllTimers();
    void onGameOver();
    
private:
    struct BattleCampInfos {
        std::vector<const UnderWorld::Core::Camp*> campsVector;
        std::set<const UnderWorld::Core::Camp*> campSet;
        std::map<const UnderWorld::Core::Camp*, UnderWorld::Core::UnitBase*> units;
    };
    
private:
    GameRenderObserver *_observer;
    Scene* _scene;
    int _mapId;
    MapLayer* _mapLayer;
    MapUILayer* _mapUILayer;
    const UnderWorld::Core::Game* _game;
    UnderWorld::Core::Commander* _commander;
    std::map<int, UnitNode*> _allUnitNodes;
    std::map<int64_t, std::pair<float, float>> _bulletParams;
    std::map<int64_t, BulletNode*> _allBulletNodes;
    std::map<int, UnderWorld::Core::Unit*> _myUnits;
    std::map<std::string, std::map<int, UnderWorld::Core::Unit*>> _myHeroes;
    std::map<int, UnderWorld::Core::Unit*> _cores;
    std::map<int, BattleCampInfos> _battleCampInfos;
    bool _paused;
    bool _isGameOver;
    int _remainingTime;
    bool _hasUpdatedBattleCampInfos;
};

#endif /* GameRender_h */
