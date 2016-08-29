//
//  CardPropertyNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardPropertyNode.h"
#include "PureNode.h"

using namespace std;

CardPropertyNode* CardPropertyNode::create(const Color4B& color)
{
    auto ret = new (nothrow) CardPropertyNode();
    if (ret && ret->init(color)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardPropertyNode::CardPropertyNode()
:_background(nullptr)
,_icon(nullptr)
,_name(nullptr)
,_data(nullptr) {}

CardPropertyNode::~CardPropertyNode()
{
    removeAllChildren();
}

bool CardPropertyNode::init(const Color4B& color)
{
    if (Node::init()) {
        static const Size size(154, 38);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        _background = PureNode::create(color, size);
        _background->setPosition(size.width / 2, size.height / 2);
        addChild(_background);
        
        return true;
    }
    
    return false;
}

void CardPropertyNode::setColor(const Color4B& color)
{
    if (_background) {
        _background->setColor(color);
    }
}

void CardPropertyNode::setProperty()
{
    
}
