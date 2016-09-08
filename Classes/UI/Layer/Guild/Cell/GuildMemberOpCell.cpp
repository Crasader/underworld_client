//
//  GuildMemberOpCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildMemberOpCell.h"
#include "UserGuildData.h"

using namespace std;

GuildMemberOpCell* GuildMemberOpCell::create()
{
    auto ret = new (nothrow) GuildMemberOpCell();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildMemberOpCell::GuildMemberOpCell()
:_observer(nullptr) {}

GuildMemberOpCell::~GuildMemberOpCell()
{
    removeAllChildren();
}

bool GuildMemberOpCell::init()
{
    if (GuildGenericCell::init())
    {
        
        return true;
    }
    
    return false;
}

void GuildMemberOpCell::registerObserver(GuildMemberOpCellObserver *observer)
{
    _observer = observer;
}

void GuildMemberOpCell::update(const UserGuildData* data)
{
    
}
