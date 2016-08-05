//
//  GuildSimpleNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildSimpleNode.h"
#include "CocosUtils.h"
#include "CocosGlobal.h"

using namespace std;

GuildSimpleNode* GuildSimpleNode::create()
{
    GuildSimpleNode *p = new (nothrow) GuildSimpleNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

GuildSimpleNode::GuildSimpleNode()
:_icon(nullptr)
,_name(nullptr) {}

GuildSimpleNode::~GuildSimpleNode()
{
    removeFromParent();
}

bool GuildSimpleNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto icon = Sprite::create(CocosUtils::getResourcePath("icon_hanghui_1_1.png"));
        addChild(icon);
        _icon = icon;
        
        auto name = CocosUtils::createLabel("guild name", SMALL_FONT_SIZE);
        name->setTextColor(Color4B::YELLOW);
        name->setHorizontalAlignment(TextHAlignment::LEFT);
        name->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        addChild(name);
        _name = name;
        
        const auto& isize(icon->getContentSize());
        const auto& nsize(name->getContentSize());
        
        static const float spaceX(5.0f);
        const Size size(isize.width, MAX(isize.height, nsize.height));
        setContentSize(size);
        icon->setPosition(size.width / 2, size.height / 2);
        name->setPosition(size.width + spaceX, size.height / 2);
        
        return true;
    }
    
    return false;
}

void GuildSimpleNode::setIcon(int idx)
{
    if (_icon) {
        _icon->setTexture(CocosUtils::getResourcePath("icon_hanghui_1_1.png"));
    }
}

void GuildSimpleNode::setGuildName(const string& name)
{
    if (_name) {
        _name->setString(name);
    }
}
