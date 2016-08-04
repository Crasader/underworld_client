//
//  TrophyGapNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TrophyGapNode.h"
#include "CocosUtils.h"

using namespace std;

TrophyGapNode* TrophyGapNode::create()
{
    TrophyGapNode *p = new (nothrow) TrophyGapNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

TrophyGapNode::TrophyGapNode()
:_trophy(nullptr) {}

TrophyGapNode::~TrophyGapNode()
{
    removeFromParent();
}

bool TrophyGapNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto icon = Sprite::create(CocosUtils::getResourcePath("icon_jiangbei_1.png"));
        addChild(icon);
        
        auto trophy = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        trophy->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        trophy->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        trophy->setTextColor(Color4B::BLACK);
        addChild(trophy);
        _trophy = trophy;
        
        const auto& isize(icon->getContentSize());
        const auto& tsize(trophy->getContentSize());
        
        static const float spaceX(5.0f);
        const Size size(isize.width, MAX(isize.height, tsize.height));
        setContentSize(size);
        icon->setPosition(size.width / 2, size.height / 2);
        trophy->setPosition(size.width + spaceX, size.height / 2);
        
        return true;
    }
    
    return false;
}

void TrophyGapNode::setCount(int count)
{
    if (_trophy) {
        _trophy->setString(StringUtils::format("%d", count));
    }
}
