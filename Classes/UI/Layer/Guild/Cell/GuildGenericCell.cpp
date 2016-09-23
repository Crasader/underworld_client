//
//  GuildGenericCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildGenericCell.h"
#include "CocosUtils.h"
#include "AvatarNode.h"
#include "GuildTorphyNode.h"

GuildGenericCell::GuildGenericCell()
:_touchInvalid(false)
,_avatar(nullptr)
,_name(nullptr)
,_description(nullptr)
,_hint(nullptr)
,_trophy(nullptr) {}

GuildGenericCell::~GuildGenericCell()
{
    removeAllChildren();
}

bool GuildGenericCell::init()
{
    static const auto& file(CocosUtils::getResourcePath("ui_tiao_15.png"));
    if (ui::Button::init(file, file)) {
        setScale9Enabled(true);
        
        auto avatar = AvatarNode::create();
        addChild(avatar);
        _avatar = avatar;
        
        {
            auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            addChild(label);
            _name = label;
        }
        
        {
            auto label = CocosUtils::createLabel("", SMALL_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            addChild(label);
            _description = label;
        }
        
        
        {
            auto label = CocosUtils::createLabel("", SMALL_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE);
            addChild(label);
            _hint = label;
        }
        
        setAvatarEdgeX(0);
        
        const auto& size(getContentSize());
        auto trophy = GuildTorphyNode::create();
        const auto& tsize(trophy->getContentSize());
        static const float tedgeRight(8);
        static const float offsetBottom(2);
        trophy->setPosition(Point(size.width - (tsize.width / 2 + tedgeRight), size.height / 2 + offsetBottom));
        addChild(trophy);
        _trophy = trophy;
        
        return true;
    }
    
    return false;
}

void GuildGenericCell::setAvatarEdgeX(float x)
{
    if (_avatar) {
        const auto& asize(_avatar->getContentSize());
        const auto& basePoint(Point(0, 8) + Point(asize.width / 2, asize.height / 2));
        auto apos(basePoint + Point(x, 0));
        _avatar->setPosition(apos);
        
        const auto& size(getContentSize());
        static const float anspace(5);
        static const float spaceY(5);
        const float leftX(apos.x + asize.width / 2 + anspace);
        
        if (_name) {
            const float height(_name->getContentSize().height);
            _name->setPosition(leftX, size.height / 2 + (height / 2 + spaceY));
        }
        
        if (_description) {
            const float height(_description->getContentSize().height);
            _description->setPosition(leftX, size.height / 2 - (height / 2 + spaceY));
        }
        
        if (_hint) {
            const float height(_hint->getContentSize().height);
            _hint->setPosition(size.width / 2, apos.y - (asize.height - height) / 2);
        }
    }
}
