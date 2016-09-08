//
//  GuildUICreateNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildUICreateNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"

using namespace std;

GuildUICreateNode* GuildUICreateNode::create(const Size& size)
{
    auto ret = new (nothrow) GuildUICreateNode();
    if (ret && ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildUICreateNode::GuildUICreateNode()
:_observer(nullptr) {}

GuildUICreateNode::~GuildUICreateNode()
{
    removeAllChildren();
}

void GuildUICreateNode::registerObserver(GuildUICreateNodeObserver *observer)
{
    _observer = observer;
}

bool GuildUICreateNode::init(const Size& size)
{
    if (Node::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        return true;
    }
    
    return false;
}
