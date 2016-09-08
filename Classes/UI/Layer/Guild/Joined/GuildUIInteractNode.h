//
//  GuildUIInteractNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildUIInteractNode_h
#define GuildUIInteractNode_h

#include "cocos2d.h"

USING_NS_CC;

class GuildData;

class GuildUIInteractNodeObserver
{
public:
    virtual ~GuildUIInteractNodeObserver() {}
};

class GuildUIInteractNode : public Node
{
public:
    static GuildUIInteractNode* create(const Size& size, const GuildData* data);
    virtual ~GuildUIInteractNode();
    void registerObserver(GuildUIInteractNodeObserver *observer);
    
protected:
    GuildUIInteractNode();
    bool init(const Size& size, const GuildData* data);
    
private:
    GuildUIInteractNodeObserver* _observer;
};

#endif /* GuildUIInteractNode_h */
