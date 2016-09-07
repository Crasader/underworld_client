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

class GuildCellObserver
{
public:
    virtual ~GuildCellObserver() {}
};

class GuildCell : public GuildGenericCell
{
public:
    static GuildCell* create();
    virtual ~GuildCell();
    void registerObserver(GuildCellObserver *observer);
    
    bool update(const GuildData* data);
    
protected:
    GuildCell();
    virtual bool init() override;
    
protected:
    GuildCellObserver* _observer;
};

#endif /* GuildCell_h */
