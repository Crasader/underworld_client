//
//  ChestInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ChestInfoLayer_h
#define ChestInfoLayer_h

#include "cocos2d.h"

USING_NS_CC;

class ChestInfoLayerObserver
{
public:
    virtual ~ChestInfoLayerObserver() {}
};

class ChestInfoLayer : public LayerColor
{
public:
    static ChestInfoLayer* create();
    virtual ~ChestInfoLayer();
    void registerObserver(ChestInfoLayerObserver *observer);
    
private:
    ChestInfoLayer();
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    ChestInfoLayerObserver* _observer;
};

#endif /* ChestInfoLayer_h */
