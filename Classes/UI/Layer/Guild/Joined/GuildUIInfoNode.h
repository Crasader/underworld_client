//
//  GuildUIInfoNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildUIInfoNode_h
#define GuildUIInfoNode_h

#include "cocos2d.h"

USING_NS_CC;

class GuildData;

class GuildUIInfoNodeObserver
{
public:
    virtual ~GuildUIInfoNodeObserver() {}
    virtual void onGuildUIInfoNodeDetail() = 0;
    virtual void onGuildUIInfoNodeFriendship() = 0;
    virtual void onGuildUIInfoNodeInteract() = 0;
    virtual void onGuildUIInfoNodeDonate() = 0;
};

class GuildUIInfoNode : public Node
{
public:
    static GuildUIInfoNode* create(const Size& size, const GuildData* data);
    virtual ~GuildUIInfoNode();
    void registerObserver(GuildUIInfoNodeObserver *observer);
    
protected:
    GuildUIInfoNode();
    bool init(const Size& size, const GuildData* data);
    
private:
    GuildUIInfoNodeObserver* _observer;
};

#endif /* GuildUIInfoNode_h */
