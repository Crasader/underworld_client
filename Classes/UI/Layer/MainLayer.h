//
//  MainLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MainLayer_h
#define MainLayer_h

#include "cocos2d.h"

USING_NS_CC;

class MainLayerObserver
{
public:
    virtual ~MainLayerObserver() {}
};

class MainLayer : public LayerColor
{
public:
    static MainLayer* create();
    virtual ~MainLayer();
    void registerObserver(MainLayerObserver *observer);
    
protected:
    MainLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    MainLayerObserver *_observer;
};

#endif /* MainLayer_h */
