//
//  GuildCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildCell.h"
#include "GuildData.h"
#include "CocosUtils.h"
#include "AvatarNode.h"
#include "GuildTorphyNode.h"

using namespace std;

GuildCell* GuildCell::create()
{
    auto ret = new (nothrow) GuildCell();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildCell::GuildCell()
:_observer(nullptr)
,_data(nullptr)
,_avatar(nullptr)
,_name(nullptr)
,_type(nullptr)
,_members(nullptr)
,_trophy(nullptr) {}

GuildCell::~GuildCell()
{
    removeAllChildren();
}

bool GuildCell::init()
{
    if (GuildGenericCell::init())
    {
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, [this](Ref*) {
            if (_observer) {
                _observer->onGuildCellTouched(_data);
            }
        });
        
        auto avatar = AvatarNode::create();
        const auto& asize(avatar->getContentSize());
        avatar->setPosition(Point(5, 8) + Point(asize.width / 2, asize.height / 2));
        addChild(avatar);
        _avatar = avatar;
        
        const auto& size(getContentSize());
        const auto& apos(avatar->getPosition());
        static const float anspace(5);
        static const float spaceY(5);
        const float leftX(apos.x + asize.width / 2 + anspace);
        {
            auto label = CocosUtils::createLabel("Clan", DEFAULT_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            const float height(label->getContentSize().height);
            label->setPosition(leftX, size.height / 2 + (height / 2 + spaceY));
            addChild(label);
            _name = label;
        }
        
        {
            auto label = CocosUtils::createLabel("Anyone can join", SMALL_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            const float height(label->getContentSize().height);
            label->setPosition(leftX, size.height / 2 - (height / 2 + spaceY));
            addChild(label);
            _type = label;
        }
        
        {
            auto label = CocosUtils::createLabel("Tap to view details", SMALL_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE);
            const float height(label->getContentSize().height);
            label->setPosition(size.width / 2, apos.y - (asize.height - height) / 2);
            addChild(label);
        }
        
        auto trophy = GuildTorphyNode::create();
        const auto& tsize(trophy->getContentSize());
        static const float tedge(5);
        trophy->setPosition(Point(size.width - (tsize.width / 2 + tedge), size.height / 2));
        addChild(trophy);
        _trophy = trophy;
        
        const auto& tpos(trophy->getPosition());
        static const float mtspace(70);
        const float rightX(tpos.x - (tsize.width / 2 + mtspace));
        {
            auto label = CocosUtils::createLabel("Members", BIG_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE);
            const float height(label->getContentSize().height);
            label->setPosition(rightX, size.height / 2 + (height / 2 + spaceY));
            addChild(label);
        }
        
        {
            auto label = CocosUtils::createLabel("45/50", SMALL_FONT_SIZE);
            label->setTextColor(Color4B::BLACK);
            label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE);
            const float height(label->getContentSize().height);
            label->setPosition(rightX, size.height / 2 - (height / 2 + spaceY));
            addChild(label);
            _members = label;
        }
        
        return true;
    }
    
    return false;
}

void GuildCell::registerObserver(GuildCellObserver *observer)
{
    _observer = observer;
}

void GuildCell::update(const GuildData* data)
{
    if (_data != data) {
        _data = data;
        
        if (_avatar) {
            _avatar->update(data ? 0 : 0);
        }
        
        if (_name) {
            _name->setString(data ? data->getName() : "");
        }
        
        if (_type) {
            _type->setString(data ? "" : "");
        }
        
        if (_members) {
            _members->setString(data ? "" : "");
        }
        
        if (_trophy) {
            _trophy->setCount(data ? data->getTotalTrophies() : 0);
        }
    }
}
