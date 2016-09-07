//
//  ChestLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ChestLayer_h
#define ChestLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "ChestNode.h"

USING_NS_CC;

class ChestLayerObserver
{
public:
    virtual ~ChestLayerObserver() {}
};

class ChestLayer
: public LayerColor
, public ChestNodeObserver
{
public:
    static ChestLayer* create();
    virtual ~ChestLayer();
    void registerObserver(ChestLayerObserver *observer);
    
protected:
    ChestLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // ChestNodeObserver
    
private:
    ChestLayerObserver *_observer;
    ui::ScrollView* _scrollView;
};

#endif /* ChestLayer_h */
