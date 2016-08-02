//
//  ExpNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ExpNode.h"
#include "CocosUtils.h"

using namespace std;

ExpNode* ExpNode::create()
{
    ExpNode *p = new (nothrow) ExpNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

ExpNode::ExpNode()
:_level(nullptr)
,_expProgress(nullptr) {}

ExpNode::~ExpNode()
{
    removeFromParent();
}

bool ExpNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto bg = Sprite::create(CocosUtils::getResourcePath("ui_tiao_1.png"));
        addChild(bg);
        
        auto s = Sprite::create(CocosUtils::getResourcePath("ui_tiao_1_1.png"));
        auto pt = ProgressTimer::create(s);
        pt->setType(ProgressTimer::Type::BAR);
        pt->setBarChangeRate(Vec2(1.0f, 0.0f));
        pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
        pt->setPercentage(80);
        bg->addChild(pt);
        _expProgress = pt;
        
        auto icon = Sprite::create("GameImages/resources/icon_104B.png");
        bg->addChild(icon);
        
        auto level = CocosUtils::createLabel("123", SMALL_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        icon->addChild(level);
        _level = level;
        
        const auto& bsize(bg->getContentSize());
        icon->setPosition(Point(0, bsize.height / 2));
        pt->setPosition(Point(bsize.width / 2, bsize.height / 2));
        
        const auto& isize(icon->getContentSize());
        level->setPosition(Point(isize.width / 2, isize.height / 2));
        
        const Size size(isize.width / 2 + bsize.width, MAX(bsize.height, isize.height));
        setContentSize(size);
        bg->setPosition((isize.width + bsize.width) / 2, size.height / 2);
        
        return true;
    }
    
    return false;
}

void ExpNode::setLevel(int level)
{
    if (_level) {
        _level->setString(StringUtils::format("%d", level));
    }
}

void ExpNode::setPercentage(float p)
{
    if (_expProgress) {
        _expProgress->setPercentage(p);
    }
}
