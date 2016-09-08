//
//  GuildUIInteractNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildUIInteractNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"

using namespace std;

GuildUIInteractNode* GuildUIInteractNode::create(const Size& size, const GuildData* data)
{
    auto ret = new (nothrow) GuildUIInteractNode();
    if (ret && ret->init(size, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildUIInteractNode::GuildUIInteractNode()
:_observer(nullptr) {}

GuildUIInteractNode::~GuildUIInteractNode()
{
    removeAllChildren();
}

void GuildUIInteractNode::registerObserver(GuildUIInteractNodeObserver *observer)
{
    _observer = observer;
}

bool GuildUIInteractNode::init(const Size& size, const GuildData* data)
{
    if (Node::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        return true;
    }
    
    return false;
}
