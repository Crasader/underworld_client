//
//  GuildUIMembersList.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildUIMembersList.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"

using namespace std;

GuildUIMembersList* GuildUIMembersList::create(const Size& size, const GuildData* data)
{
    auto ret = new (nothrow) GuildUIMembersList();
    if (ret && ret->init(size, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildUIMembersList::GuildUIMembersList()
:_observer(nullptr) {}

GuildUIMembersList::~GuildUIMembersList()
{
    removeAllChildren();
}

void GuildUIMembersList::registerObserver(GuildUIMembersListObserver *observer)
{
    _observer = observer;
}

bool GuildUIMembersList::init(const Size& size, const GuildData* data)
{
    if (Node::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        return true;
    }
    
    return false;
}
