//
//  ProgressLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ProgressLayer_h
#define ProgressLayer_h

#include "cocos2d.h"

USING_NS_CC;

class ProgressLayer : public LayerColor
{
public:
    static void showSingleton();
    static void hideSingleton();
    
    static ProgressLayer* create();
    static void destroy(ProgressLayer* layer);
    
protected:
    ProgressLayer();
    virtual ~ProgressLayer();
    
public:
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
};

#endif /* ProgressLayer_h */
