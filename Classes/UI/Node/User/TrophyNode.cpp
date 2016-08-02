//
//  TrophyNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TrophyNode.h"
#include "CocosUtils.h"

using namespace std;

TrophyNode* TrophyNode::create()
{
    TrophyNode *p = new (nothrow) TrophyNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

TrophyNode::TrophyNode()
:_trophy(nullptr) {}

TrophyNode::~TrophyNode()
{
    removeFromParent();
}

bool TrophyNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto bg = Sprite::create(CocosUtils::getResourcePath("ui_tiao_2.png"));
        addChild(bg);
        
        auto icon = Sprite::create(CocosUtils::getResourcePath("icon_jiangbei_1.png"));
        bg->addChild(icon);
        
        auto trophy = CocosUtils::createLabel("0", SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        trophy->setAnchorPoint(Point::ANCHOR_MIDDLE);
        bg->addChild(trophy);
        _trophy = trophy;
        
        const auto& bsize(bg->getContentSize());
        const auto& isize(icon->getContentSize());
        icon->setPosition(Point(0, bsize.height / 2));
        trophy->setPosition(Point(bsize.width / 2 + isize.width / 4, bsize.height / 2));
        
        const Size size(isize.width / 2 + bsize.width, MAX(bsize.height, isize.height));
        setContentSize(size);
        bg->setPosition((isize.width + bsize.width) / 2, size.height / 2);
        
        return true;
    }
    
    return false;
}

void TrophyNode::setCount(int count)
{
    if (_trophy) {
        _trophy->setString(StringUtils::format("%d", count));
    }
}
