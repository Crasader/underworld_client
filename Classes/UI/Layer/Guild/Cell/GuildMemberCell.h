//
//  GuildMemberCell.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildMemberCell_h
#define GuildMemberCell_h

#include "GuildGenericCell.h"

class UserGuildData;

class GuildMemberCellObserver
{
public:
    virtual ~GuildMemberCellObserver() {}
};

class GuildMemberCell : public GuildGenericCell
{
public:
    static GuildMemberCell* create();
    virtual ~GuildMemberCell();
    void registerObserver(GuildMemberCellObserver *observer);
    
    bool update(const UserGuildData* data);
    
protected:
    GuildMemberCell();
    virtual bool init() override;
    
protected:
    GuildMemberCellObserver* _observer;
};

#endif /* GuildMemberCell_h */
