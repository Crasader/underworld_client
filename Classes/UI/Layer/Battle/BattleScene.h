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
    
private:
    int _mapId;
    GameRender* _render;
    UnderWorld::Core::GameLooper* _looper;
    GameScheduler* _sch;
};

#endif /* BattleScene_h */
