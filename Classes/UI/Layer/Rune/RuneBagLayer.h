//
//  RuneBagLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneBagLayer_h
#define RuneBagLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class RuneBagLayerObserver
{
public:
    virtual ~RuneBagLayerObserver() {}
};

class RuneBagLayer : public LayerColor
{
public:
    static RuneBagLayer* create();
    virtual ~RuneBagLayer();
    void registerObserver(RuneBagLayerObserver *observer);
    
private:
    RuneBagLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    RuneBagLayerObserver* _observer;
};

#endif /* RuneBagLayer_h */
