//
//  GuildLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildLayer_h
#define GuildLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "GuildUIInfoNode.h"
#include "GuildUIInteractNode.h"
#include "GuildUIDonateNode.h"

USING_NS_CC;

class GuildLayerObserver
{
public:
    virtual ~GuildLayerObserver() {}
};

class GuildLayer
: public LayerColor
, public GuildUIInfoNodeObserver
, public GuildUIInteractNodeObserver
, public GuildUIDonateNodeObserver
{
public:
    static GuildLayer* create();
    virtual ~GuildLayer();
    
private:
    GuildLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // GuildUIInfoNodeObserver
    virtual void onGuildUIInfoNodeDetail() override;
    virtual void onGuildUIInfoNodeFriendship() override;
    virtual void onGuildUIInfoNodeInteract() override;
    virtual void onGuildUIInfoNodeDonate() override;
    
    // GuildUIInteractNodeObserver
    
    // GuildUIDonateNodeObserver
    
private:
    Node* _rightBackground;
    GuildUIInfoNode* _infoUINode;
    GuildUIInteractNode* _redEnvelopeUINode;
    GuildUIDonateNode* _donateUINode;
};

#endif /* GuildLayer_h */
