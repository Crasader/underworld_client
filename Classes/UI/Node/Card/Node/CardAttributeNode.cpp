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

void CardAttributeNode::setAttribute(ObjectUtils::CardAttributeType type, float value)
{
    string icon;
    string name;
    getInfo(type, icon, name);
    if (_icon) {
        _icon->setTexture(icon);
    }
    
    setName(name);
    
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
                ostringstream os;
                os << value;
                key = os.str();
                break;
        }
        
        _data->setString(LocalHelper::getString(key));
    }
}

void CardAttributeNode::setName(const string& name)
{
    if (_name) {
        _name->setString(name);
    }
}

void CardAttributeNode::getInfo(ObjectUtils::CardAttributeType type, string& icon, string& name) const
{
    string iconFile;
    string nameSuffix;
    switch (type) {
        case ObjectUtils::CardAttributeType::HP: {
            iconFile = "icon_hp.png";
            nameSuffix = "hp";
        }
            break;
        case ObjectUtils::CardAttributeType::SPEED: {
            iconFile = "icon_sudu.png";
            nameSuffix = "speed";
        }
            break;
        case ObjectUtils::CardAttributeType::ARMOR: {
            iconFile = "icon_hujiazhi.png";
            nameSuffix = "armor";
        }
            break;
        case ObjectUtils::CardAttributeType::ARMOR_TYPE: {
            iconFile = "icon_hujialeixing.png";
            nameSuffix = "armorType";
        }
            break;
        case ObjectUtils::CardAttributeType::GROUND_DAMAGE: {
            iconFile = "icon_gongjili.png";
            nameSuffix = "groundDamage";
        }
            break;
        case ObjectUtils::CardAttributeType::ATTACK_TYPE: {
            iconFile = "icon_gongjifanwei.png";
            nameSuffix = "attackType";
        }
            break;
        case ObjectUtils::CardAttributeType::HIT_SPEED: {
            iconFile = "icon_gongjisudu.png";
            nameSuffix = "hitSpeed";
        }
            break;
        case ObjectUtils::CardAttributeType::RANGE: {
            iconFile = "icon_gongjifanwei.png";
            nameSuffix = "range";
        }
            break;
        case ObjectUtils::CardAttributeType::TARGET_TYPE: {
            iconFile = "icon_gongjileixing.png";
            nameSuffix = "targetType";
        }
            break;
        case ObjectUtils::CardAttributeType::AIR_DAMAGE: {
            iconFile = "icon_gongjili.png";
            nameSuffix = "airDamage";
        }
            break;
        default:
            break;
    }
    
    icon.assign(iconFile.empty() ? "" : CocosUtils::getResourcePath(iconFile));
    name.assign(nameSuffix.empty() ? "" : LocalHelper::getString("ui_cardAttr_" + nameSuffix));
}
