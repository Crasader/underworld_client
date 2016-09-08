//
//  GuildMemberOpCell.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildMemberOpCell_h
#define GuildMemberOpCell_h

#include "GuildGenericCell.h"

class UserGuildData;

class GuildMemberOpCellObserver
{
public:
    virtual ~GuildMemberOpCellObserver() {}
};

class GuildMemberOpCell : public GuildGenericCell
{
public:
    static GuildMemberOpCell* create();
    virtual ~GuildMemberOpCell();
    void registerObserver(GuildMemberOpCellObserver *observer);
    
    void update(const UserGuildData* data);
    
protected:
    GuildMemberOpCell();
    virtual bool init() override;
    
protected:
    GuildMemberOpCellObserver* _observer;
};

#endif /* GuildMemberOpCell_h */
