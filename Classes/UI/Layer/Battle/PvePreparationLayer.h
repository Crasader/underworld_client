//
//  PvePreparationLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/11/6.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef PvePreparationLayer_h
#define PvePreparationLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class PvePreparationLayerObserver
{
public:
    virtual ~PvePreparationLayerObserver() {}
};

class PvePreparationLayer : public LayerColor
{
public:
    static PvePreparationLayer* create(int levelId);
    virtual ~PvePreparationLayer();
    void registerObserver(PvePreparationLayerObserver *observer);
    
protected:
    PvePreparationLayer();
    bool init(int levelId);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    PvePreparationLayerObserver *_observer;
};

#endif /* PvePreparationLayer_h */
