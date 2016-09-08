//
//  GuildMemberLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildMemberLayer_h
#define GuildMemberLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UserGuildData;

class GuildMemberLayerObserver
{
public:
    virtual ~GuildMemberLayerObserver() {}
};

class GuildMemberLayer : public LayerColor
{
public:
    static GuildMemberLayer* create(const UserGuildData* data);
    virtual ~GuildMemberLayer();
    void registerObserver(GuildMemberLayerObserver *observer);
    
private:
    GuildMemberLayer();
    // LayerColor
    bool init(const UserGuildData* data);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    GuildMemberLayerObserver* _observer;
    const UserGuildData* _data;
};

#endif /* GuildMemberLayer_h */
