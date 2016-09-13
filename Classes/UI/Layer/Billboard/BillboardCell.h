//
//  BillboardCell.h
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BillboardCell_h
#define BillboardCell_h

#include "GuildGenericCell.h"

class BillboardData;

class BillboardCellObserver
{
public:
    virtual ~BillboardCellObserver() {}
    virtual void onBillboardCellClicked(const BillboardData* data) = 0;
};

class BillboardCell : public GuildGenericCell
{
public:
    static BillboardCell* create();
    virtual ~BillboardCell();
    void registerObserver(BillboardCellObserver *observer);
    
    void update(const BillboardData* data);
    
protected:
    BillboardCell();
    virtual bool init() override;
    
protected:
    BillboardCellObserver* _observer;
    const BillboardData* _data;
    Sprite* _topRankBg;
    Label* _rank;
};

#endif /* BillboardCell_h */
