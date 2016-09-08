//
//  GuildUIMembersList.h
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildUIMembersList_h
#define GuildUIMembersList_h

#include "cocos2d.h"

USING_NS_CC;

class GuildData;

class GuildUIMembersListObserver
{
public:
    virtual ~GuildUIMembersListObserver() {}
};

class GuildUIMembersList : public Node
{
public:
    static GuildUIMembersList* create(const Size& size, const GuildData* data);
    virtual ~GuildUIMembersList();
    void registerObserver(GuildUIMembersListObserver *observer);
    
protected:
    GuildUIMembersList();
    bool init(const Size& size, const GuildData* data);
    
private:
    GuildUIMembersListObserver* _observer;
};

#endif /* GuildUIMembersList_h */
