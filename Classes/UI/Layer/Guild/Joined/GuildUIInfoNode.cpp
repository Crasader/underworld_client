//
//  GuildUIInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildUIInfoNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"

using namespace std;

GuildUIInfoNode* GuildUIInfoNode::create(const Size& size, const GuildData* data)
{
    auto ret = new (nothrow) GuildUIInfoNode();
    if (ret && ret->init(size, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildUIInfoNode::GuildUIInfoNode()
:_observer(nullptr) {}

GuildUIInfoNode::~GuildUIInfoNode()
{
    removeAllChildren();
}

void GuildUIInfoNode::registerObserver(GuildUIInfoNodeObserver *observer)
{
    _observer = observer;
}

bool GuildUIInfoNode::init(const Size& size, const GuildData* data)
{
    if (Node::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        return true;
    }
    
    return false;
}
