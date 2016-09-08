//
//  GuildMemberCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildMemberCell.h"
#include "UserGuildData.h"

using namespace std;

GuildMemberCell* GuildMemberCell::create()
{
    auto ret = new (nothrow) GuildMemberCell();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildMemberCell::GuildMemberCell()
:_observer(nullptr) {}

GuildMemberCell::~GuildMemberCell()
{
    removeAllChildren();
}

bool GuildMemberCell::init()
{
    if (GuildGenericCell::init())
    {
        
        return true;
    }
    
    return false;
}

void GuildMemberCell::registerObserver(GuildMemberCellObserver *observer)
{
    _observer = observer;
}

void GuildMemberCell::update(const UserGuildData* data)
{
    
}
