//
//  ObjectBriefNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ObjectBriefNode.h"
#include "CocosUtils.h"
#include "ObjectBriefData.h"

ObjectBriefNode::ObjectBriefNode()
:_icon(nullptr)
,_count(nullptr)
,_data(nullptr) {}

ObjectBriefNode::~ObjectBriefNode()
{
    removeFromParent();
}

ObjectBriefNode* ObjectBriefNode::create()
{
    auto ret = new (std::nothrow) ObjectBriefNode();
    if(ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ObjectBriefNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        _icon = Sprite::create("GameImages/resources/icon_102S.png");
        addChild(_icon);
        
        _count = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _count->setHorizontalAlignment(TextHAlignment::LEFT);
        _count->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        addChild(_count);
        
        return true;
    }
    
    return false;
}

void ObjectBriefNode::update(const ObjectBriefData* data)
{
    if (data) {
        static const Size iconSize(20, 20);
        _icon->setTexture(StringUtils::format("GameImages/resources/icon_%dS.png", data->getId()));
        const auto& isize(_icon->getContentSize());
        _icon->setScale(iconSize.width / isize.width, iconSize.height / isize.height);
        
        _count->setString(StringUtils::format("X%d", data->getCount()));
        
        const auto& csize(_count->getContentSize());
        static const float space(3.0f);
        const Size size(iconSize.width + space + csize.width, iconSize.height);
        setContentSize(size);
        _icon->setPosition(Point(iconSize.width / 2, size.height / 2));
        _count->setPosition(Point(size.width - csize.width, size.height / 2));
    }
}

const ObjectBriefData* ObjectBriefNode::getData() const
{
    return _data;
}
