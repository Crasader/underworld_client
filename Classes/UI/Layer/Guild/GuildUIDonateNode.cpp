//
//  GuildUIDonateNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildUIDonateNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"

using namespace std;

GuildUIDonateNode* GuildUIDonateNode::create(const Size& size, const GuildData* data)
{
    auto ret = new (nothrow) GuildUIDonateNode();
    if (ret && ret->init(size, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildUIDonateNode::GuildUIDonateNode()
:_observer(nullptr) {}

GuildUIDonateNode::~GuildUIDonateNode()
{
    removeAllChildren();
}

void GuildUIDonateNode::registerObserver(GuildUIDonateNodeObserver *observer)
{
    _observer = observer;
}

bool GuildUIDonateNode::init(const Size& size, const GuildData* data)
{
    if (Node::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        return true;
    }
    
    return false;
}
