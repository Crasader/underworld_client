//
//  GuildCell.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildCell_h
#define GuildCell_h

#include "GuildGenericCell.h"

class GuildData;
class AvatarNode;
class GuildTorphyNode;

class GuildCellObserver
{
public:
    virtual ~GuildCellObserver() {}
    virtual void onGuildCellTouched(const GuildData* data) = 0;
};

class GuildCell : public GuildGenericCell
{
public:
    static GuildCell* create();
    virtual ~GuildCell();
    void registerObserver(GuildCellObserver *observer);
    
    void update(const GuildData* data);
    
protected:
    GuildCell();
    virtual bool init() override;
    
protected:
    GuildCellObserver* _observer;
    const GuildData* _data;
    AvatarNode* _avatar;
    Label* _name;
    Label* _type;
    Label* _members;
    GuildTorphyNode* _trophy;
};

#endif /* GuildCell_h */
