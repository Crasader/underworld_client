//
//  GuildInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildInfoLayer_h
#define GuildInfoLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class GuildInfoLayerObserver
{
public:
    virtual ~GuildInfoLayerObserver() {}
};

class GuildInfoLayer : public LayerColor
{
public:
    static GuildInfoLayer* create();
    virtual ~GuildInfoLayer();
    
private:
    GuildInfoLayer();
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    
};

#endif /* GuildInfoLayer_h */
