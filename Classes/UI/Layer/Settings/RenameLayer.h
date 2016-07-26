//
//  RenameLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RenameLayer_h
#define RenameLayer_h

#include "cocos2d.h"

USING_NS_CC;

class RenameLayerObserver
{
public:
    virtual ~RenameLayerObserver() {}
};

class RenameLayer : public LayerColor
{
public:
    static RenameLayer* create();
    virtual ~RenameLayer();
    void registerObserver(RenameLayerObserver *observer);
    
protected:
    RenameLayer();
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    RenameLayerObserver *_observer;
};

#endif /* RenameLayer_h */
