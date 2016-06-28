//
//  XCardNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "XCardNode.h"
#include "CocosUtils.h"

using namespace std;

XCardNode* XCardNode::create()
{
    XCardNode *ret = new (nothrow) XCardNode();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

XCardNode::XCardNode()
:_cardNode(nullptr)
,_countLabel(nullptr) {}

XCardNode::~XCardNode()
{
    removeAllChildren();
}

bool XCardNode::init()
{
    if (Node::init())
    {
        _cardNode = CardNode::create(false);
        addChild(_cardNode);
        
        auto sprite = Sprite::create("GameImages/public/ui_green_2.png");
        addChild(sprite);
        _countLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT );
        sprite->addChild(_countLabel);
        const auto& spriteSize(sprite->getContentSize());
        _countLabel->setPosition(Point(spriteSize.width / 2, spriteSize.height / 2));
        
        static float offsetY(6.0f);
        const auto& cardSize = _cardNode->getContentSize();
        
        Size size(cardSize);
        size.height = size.height + offsetY + spriteSize.height;
        
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        _cardNode->setPosition(Point(size.width / 2, size.height - cardSize.height / 2));
        sprite->setPosition(Point(size.width / 2, spriteSize.height / 2));
        
        return true;
    }
    
    return false;
}

CardNode* XCardNode::getCardNode() const
{
    return _cardNode;
}

void XCardNode::updateCount(int count)
{
    if (_countLabel) {
        _countLabel->setString(StringUtils::format("%d/%d", count, 10));
    }
}
