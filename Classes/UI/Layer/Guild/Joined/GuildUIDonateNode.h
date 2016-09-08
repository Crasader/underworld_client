//
//  GuildUIDonateNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildUIDonateNode_h
#define GuildUIDonateNode_h

#include "cocos2d.h"

USING_NS_CC;

class GuildData;

class GuildUIDonateNodeObserver
{
public:
    virtual ~GuildUIDonateNodeObserver() {}
};

class GuildUIDonateNode : public Node
{
public:
    static GuildUIDonateNode* create(const Size& size, const GuildData* data);
    virtual ~GuildUIDonateNode();
    void registerObserver(GuildUIDonateNodeObserver *observer);
    
protected:
    GuildUIDonateNode();
    bool init(const Size& size, const GuildData* data);
    
private:
    GuildUIDonateNodeObserver* _observer;
};

#endif /* GuildUIDonateNode_h */
