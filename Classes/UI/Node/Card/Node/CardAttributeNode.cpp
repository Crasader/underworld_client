//
//  CardAttributeNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardAttributeNode.h"
#include "PureNode.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include <sstream>

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
        auto icon = Sprite::create(getIconFile(ObjectUtils::CardAttributeType::HP));
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

void CardAttributeNode::setAttribute(ObjectUtils::CardAttributeType type, float value)
{
    const auto& file(getIconFile(type));
    if (_icon) {
        _icon->setTexture(file);
    }
    
    if (_name) {
        _name->setString(LocalHelper::getString(StringUtils::format("ui_cardAttr_%d", static_cast<int>(type))));
    }
    
    if (_data) {
        string key("");
        switch (type) {
            case ObjectUtils::CardAttributeType::ARMOR_TYPE:
                key = StringUtils::format("ui_cardArmorType_%d", static_cast<int>(value));
                break;
            case ObjectUtils::CardAttributeType::ATTACK_TYPE:
                key = StringUtils::format("ui_cardAttackType_%d", static_cast<int>(value));
                break;
            case ObjectUtils::CardAttributeType::TARGET_TYPE:
                key = StringUtils::format("ui_cardTarget_%d", static_cast<int>(value));
                break;
            default:
                key = (ostringstream() << value).str();
                break;
        }
        
        _data->setString(LocalHelper::getString(key));
    }
}

string CardAttributeNode::getIconFile(ObjectUtils::CardAttributeType type) const
{
    switch (type) {
        case ObjectUtils::CardAttributeType::HP:
            return CocosUtils::getResourcePath("icon_hp.png");
        case ObjectUtils::CardAttributeType::SPEED:
            return CocosUtils::getResourcePath("icon_sudu.png");
        case ObjectUtils::CardAttributeType::ARMOR:
            return CocosUtils::getResourcePath("icon_hujiazhi.png");
        case ObjectUtils::CardAttributeType::ARMOR_TYPE:
            return CocosUtils::getResourcePath("icon_hujialeixing.png");
        case ObjectUtils::CardAttributeType::GROUND_DAMAGE:
            return CocosUtils::getResourcePath("icon_gongjili.png");
        case ObjectUtils::CardAttributeType::ATTACK_TYPE:
            return CocosUtils::getResourcePath("icon_gongjifanwei.png");
        case ObjectUtils::CardAttributeType::HIT_SPEED:
            return CocosUtils::getResourcePath("icon_gongjisudu.png");
        case ObjectUtils::CardAttributeType::RANGE:
            return CocosUtils::getResourcePath("icon_gongjifanwei.png");
        case ObjectUtils::CardAttributeType::TARGET_TYPE:
            return CocosUtils::getResourcePath("icon_gongjileixing.png");
        case ObjectUtils::CardAttributeType::AIR_DAMAGE:
            return CocosUtils::getResourcePath("icon_gongjili.png");
        default:
            return "";
    }
}
