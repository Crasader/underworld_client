//
//  TowerSimpleNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TowerSimpleNode.h"
#include "CocosUtils.h"
#include "CocosGlobal.h"
#include "PvpLogUI.h"

using namespace std;

TowerSimpleNode* TowerSimpleNode::create()
{
    TowerSimpleNode *p = new (nothrow) TowerSimpleNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

TowerSimpleNode::TowerSimpleNode()
:_percentage(nullptr) {}

TowerSimpleNode::~TowerSimpleNode()
{
    removeFromParent();
}

bool TowerSimpleNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto icon = Sprite::create(PvpLogUI::getResourcePath("icon_chengbao.png"));
        addChild(icon);
        
        auto label = CocosUtils::createLabel("30%", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        addChild(label);
        _percentage = label;
        
        const auto& isize(icon->getContentSize());
        const auto& lsize(label->getContentSize());
        
        static const float spaceY(5.0f);
        const Size size(isize.width, isize.height + lsize.height + spaceY);
        setContentSize(size);
        icon->setPosition(size.width / 2, isize.height / 2);
        label->setPosition(size.width / 2, size.height - lsize.height / 2);
        
        return true;
    }
    
    return false;
}

void TowerSimpleNode::setWin(bool win)
{
    if (_percentage) {
        _percentage->setTextColor(win ? Color4B::GREEN : Color4B::RED);
    }
}

void TowerSimpleNode::setPercentage(int p)
{
    if (_percentage) {
        _percentage->setString(StringUtils::format("%d%%", p));
    }
}
