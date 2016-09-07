//
//  GuildCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildCell.h"
#include "GuildData.h"

using namespace std;

GuildCell* GuildCell::create()
{
    auto ret = new (nothrow) GuildCell();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildCell::GuildCell()
:_observer(nullptr) {}

GuildCell::~GuildCell()
{
    removeAllChildren();
}

bool GuildCell::init()
{
    if (GuildGenericCell::init())
    {
        
        return true;
    }
    
    return false;
}

void GuildCell::registerObserver(GuildCellObserver *observer)
{
    _observer = observer;
}

bool GuildCell::update(const GuildData* data)
{
    
}
