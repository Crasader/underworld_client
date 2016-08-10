//
//  MainScene.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef MainScene_h
#define MainScene_h

#include "cocos2d.h"

USING_NS_CC;

class MainScene : public Scene
{
public:
    static MainScene* create();
    virtual ~MainScene();
    
private:
    MainScene();
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
};

#endif /* MainScene_h */
