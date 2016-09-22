//
//  CardAttributeNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardAttributeNode.h"
#include "PureNode.h"
#include "Utils.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "DataManager.h"
#include "AttributeProperty.h"

using namespace std;

CardAttributeNode* CardAttributeNode::create(const Color4B& color)
{
    auto ret = new (nothrow) CardAttributeNode();
    if (ret && ret->init(color)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardAttributeNode::CardAttributeNode()
:_background(nullptr)
,_icon(nullptr)
,_name(nullptr)
,_data(nullptr) {}

CardAttributeNode::~CardAttributeNode()
{
    removeAllChildren();
}

bool CardAttributeNode::init(const Color4B& color)
{
    if (Node::init()) {
        static const Size size(154, 38);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        auto bg = PureNode::create(color, size);
        bg->setPosition(size.width / 2, size.height / 2);
        addChild(bg);
        _background = bg;
        
        static const float edgeX(2);
        auto icon = Sprite::create(CocosUtils::getResourcePath("icon_hp.png"));
        icon->setScale(0.4f);
        const Size isize(icon->getContentSize() * icon->getScale());
        icon->setPosition(Point(edgeX + isize.width / 2, size.height / 2));
        bg->addChild(icon);
        _icon = icon;
        
        const auto& ipos(icon->getPosition());
        static const float inspace(5);
        static const float spaceY(0);
        const float leftX(ipos.x + isize.width / 2 + inspace);
        
        auto name = CocosUtils::createLabel("", SMALL_FONT_SIZE);
        name->setTextColor(Color4B::BLACK);
        name->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        name->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        name->setPosition(leftX, size.height / 2 + (name->getContentSize().height / 2 + spaceY));
        addChild(name);
        _name = name;
        
        auto data = CocosUtils::createLabel("", 16);
        data->setTextColor(Color4B::BLACK);
        data->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
        data->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        data->setPosition(leftX, size.height / 2 - (data->getContentSize().height / 2 + spaceY));
        addChild(data);
        _data = data;
        
        return true;
    }
    
    return false;
}

void CardAttributeNode::setColor(const Color4B& color)
{
    if (_background) {
        _background->setColor(color);
    }
}

void CardAttributeNode::setAttribute(int attribute, float value)
{
    do {
        auto property(DataManager::getInstance()->getAttributeProperty(attribute));
        CC_BREAK_IF(!property);
        if (_icon) {
            _icon->setTexture(CocosUtils::getResourcePath(property->getIcon()));
        }
        setName(LocalHelper::getString(property->getName()));
        if (_data) {
            string msg("");
            switch (property->getType()) {
                case AttributeProperty::Type::ABSOLUTE:
                    msg = Utils::toString(value);
                    break;
                case AttributeProperty::Type::RELATIVE:
                    msg = Utils::toString(value) + "%";
                    break;
                case AttributeProperty::Type::ENUM:
                    msg = LocalHelper::getString(property->getEnmuPrefix() + Utils::toString(static_cast<int>(value)));
                    break;
            }
            _data->setString(msg);
        }
    } while (false);
}

void CardAttributeNode::setName(const string& name)
{
    if (_name) {
        _name->setString(name);
    }
}
