//
//  GuildUICreateNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildUICreateNode_h
#define GuildUICreateNode_h

#include "cocos2d.h"

USING_NS_CC;

class GuildUICreateNodeObserver
{
public:
    virtual ~GuildUICreateNodeObserver() {}
};

class GuildUICreateNode : public Node
{
public:
    static GuildUICreateNode* create(const Size& size);
    virtual ~GuildUICreateNode();
    void registerObserver(GuildUICreateNodeObserver *observer);
    
protected:
    GuildUICreateNode();
    bool init(const Size& size);
    
private:
    GuildUICreateNodeObserver* _observer;
};

#endif /* GuildUICreateNode_h */
