//
//  LaunchScene.h
//  Underworld_Client
//
//  Created by Andy on 2016/9/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef LaunchScene_h
#define LaunchScene_h

#include "cocos2d.h"

USING_NS_CC;

class LaunchScene : public Scene
{
public:
    static LaunchScene* create();
    virtual ~LaunchScene();
    
private:
    LaunchScene();
    virtual bool init() override;
};

#endif /* LaunchScene_h */
