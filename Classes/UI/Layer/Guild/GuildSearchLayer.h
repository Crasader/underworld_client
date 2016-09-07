//
//  GuildSearchLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/6.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildSearchLayer_h
#define GuildSearchLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class GuildSearchLayerObserver
{
public:
    virtual ~GuildSearchLayerObserver() {}
};

class GuildSearchLayer : public LayerColor
{
public:
    static GuildSearchLayer* create();
    virtual ~GuildSearchLayer();
    
private:
    GuildSearchLayer();
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    
};

#endif /* GuildSearchLayer_h */
