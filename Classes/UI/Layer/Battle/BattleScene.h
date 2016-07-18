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

USING_NS_CC;

class MapLayer;

class BattleScene : public Scene
{
public:
    static BattleScene *create();
    virtual ~BattleScene();
    
private:
    BattleScene();
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
};

#endif /* BattleScene_h */
