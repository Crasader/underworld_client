//
//  PveInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/17.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef PveInfoLayer_h
#define PveInfoLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class PveInfoLayerObserver
{
public:
    virtual ~PveInfoLayerObserver() {}
    virtual void onPveInfoLayerClosed(Layer* pSender) = 0;
    virtual void onPveInfoLayerContinued(Layer* pSender) = 0;
};

class PveInfoLayer : public LayerColor
{
public:
    static PveInfoLayer* create(int levelId);
    virtual ~PveInfoLayer();
    void registerObserver(PveInfoLayerObserver *observer);
    
protected:
    PveInfoLayer();
    bool init(int levelId);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    PveInfoLayerObserver *_observer;
};

#endif /* PveInfoLayer_h */
