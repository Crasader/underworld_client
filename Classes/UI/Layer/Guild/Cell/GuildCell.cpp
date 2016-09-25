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
,_members(nullptr) {}

GuildCell::~GuildCell()
{
    removeAllChildren();
}

bool GuildCell::init()
{
    if (GuildGenericCell::init())
    {
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, [this](Ref*) {
            if (_observer) {
                _observer->onGuildCellTouched(_data);
            }
        });
        
        if (_name) {
            _name->setString("Clan");
        }
        
        if (_description) {
            _description->setString("Anyone can join");
        }
        
        if (_hint) {
            _hint->setString("Tap to view details");
        }
        
        setAvatarEdgeX(5);
        
        static const float spaceY(5);
        const auto& size(getContentSize());
        const auto& tpos(_trophy->getPosition());
        const auto& tsize(_trophy->getContentSize());
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
        
        if (_description) {
            _description->setString(data ? "" : "");
        }
        
        if (_members) {
            _members->setString(data ? "" : "");
        }
        
        if (_trophy) {
            _trophy->setCount(data ? data->getTotalTrophies() : 0);
        }
    }
}
