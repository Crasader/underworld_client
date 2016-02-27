//
//  BattleScene.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleScene_h
#define BattleScene_h

#include "cocos2d.h"
#include "GameLooper.h"
#include "GameScheduler.h"
#include "GameRender.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class TechTree;
}}

class MapLayer;

class BattleScene : public Scene, public GameRenderObserver
{
public:
    static BattleScene *create(int mapId);
    virtual ~BattleScene();
    
private:
    BattleScene();
    bool init(int mapId);
    virtual void onEnter() override;
    virtual void onExit() override;
    
    // GameRenderObserver
    virtual void onGameRenderRestart() override;
    
    void start();
    void clear();
    
    bool isHero(const std::string& name) const;
    std::string getTalentUnitName(const std::string& name) const;
    void createUnitSetting(const std::string& name, UnderWorld::Core::UnitSetting& output);
    
private:
    int _mapId;
    GameRender* _render;
    UnderWorld::Core::GameLooper* _looper;
    GameScheduler* _sch;
    UnderWorld::Core::TechTree* _techTree;
};

#endif /* BattleScene_h */
