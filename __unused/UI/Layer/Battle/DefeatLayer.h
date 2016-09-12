//
//  DefeatLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef DefeatLayer_h
#define DefeatLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class DefeatLayerObserver
{
public:
    virtual ~DefeatLayerObserver() {}
    virtual void onDefeatLayerClosed(Layer* pSender) = 0;
    virtual void onDefeatLayerContinued(Layer* pSender) = 0;
};

class DefeatLayer : public LayerColor
{
public:
    static DefeatLayer* create(int levelId);
    virtual ~DefeatLayer();
    void registerObserver(DefeatLayerObserver *observer);
    
protected:
    DefeatLayer();
    bool init(int levelId);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    DefeatLayerObserver *_observer;
};

#endif /* DefeatLayer_h */
