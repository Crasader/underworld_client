//
//  CardPropertyNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardPropertyNode.h"

using namespace std;

CardPropertyNode* CardPropertyNode::create()
{
    auto ret = new (nothrow) CardPropertyNode();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardPropertyNode::CardPropertyNode()
:_icon(nullptr)
,_name(nullptr)
,_data(nullptr) {}

CardPropertyNode::~CardPropertyNode()
{
    removeAllChildren();
}

bool CardPropertyNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        return true;
    }
    
    return false;
}

void CardPropertyNode::setProperty()
{
    
}
