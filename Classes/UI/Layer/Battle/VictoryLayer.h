//
//  VictoryLayer.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef VictoryLayer_h
#define VictoryLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class VictoryLayerObserver
{
public:
    virtual ~VictoryLayerObserver() {}
};

class VictoryLayer : public LayerColor
{
public:
    static VictoryLayer* create(int levelId);
    virtual ~VictoryLayer();
    void registerObserver(VictoryLayerObserver *observer);
    
protected:
    VictoryLayer();
    bool init(int levelId);
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    VictoryLayerObserver *_observer;
};

#endif /* VictoryLayer_h */
