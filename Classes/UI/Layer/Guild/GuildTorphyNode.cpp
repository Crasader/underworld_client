//
//  GuildTorphyNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildTorphyNode.h"
#include "CocosUtils.h"

using namespace std;

GuildTorphyNode* GuildTorphyNode::create()
{
    GuildTorphyNode *p = new (nothrow) GuildTorphyNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

GuildTorphyNode::GuildTorphyNode()
:_trophy(nullptr) {}

GuildTorphyNode::~GuildTorphyNode()
{
    removeFromParent();
}

bool GuildTorphyNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto bg = Sprite::create(CocosUtils::getResourcePath("ui_kuang_8.png"));
        addChild(bg);
        
        auto icon = Sprite::create(CocosUtils::getResourcePath("icon_jiangbei.png"));
        bg->addChild(icon);
        
        auto trophy = CocosUtils::createLabel("0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        trophy->setAnchorPoint(Point::ANCHOR_MIDDLE);
        bg->addChild(trophy);
        _trophy = trophy;
        
        const auto& size(bg->getContentSize());
        bg->setPosition(size.width / 2, size.height / 2);
        setContentSize(size);
        
        const auto& isize(icon->getContentSize());
        static const float iedge(5);
        icon->setPosition(Point(size.width - (isize.width / 2 + iedge), size.height / 2));
        trophy->setPosition(Point(size.width / 2 - (isize.width / 2 + iedge), size.height / 2));
        
        return true;
    }
    
    return false;
}

void GuildTorphyNode::setCount(int count)
{
    if (_trophy) {
        _trophy->setString(StringUtils::format("%d", count));
    }
}
