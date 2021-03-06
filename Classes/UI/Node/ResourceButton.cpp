//
//  ResourceButton.cpp
//  Underworld_Client
//
//  Created by Andy on 15/7/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ResourceButton.h"
#include "CocosUtils.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

ResourceButton::ResourceButton()
:_isBigSize(false)
,_animated(false)
,_resourceEnough(true)
,_type(ResourceType::Gold)
,_count(INVALID_VALUE)
,_color(Color4B::WHITE)
,_icon(nullptr)
,_iconNode(nullptr)
,_countLabel(nullptr) {}

ResourceButton::~ResourceButton()
{
    removeAllChildren();
}

ResourceButton* ResourceButton::create(bool isBigSize, bool animated, ResourceType type, int count, const Color4B& color)
{
    auto p = new (std::nothrow) ResourceButton();
    if(p && p->init(isBigSize, animated, type, count, color)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool ResourceButton::init(bool isBigSize, bool animated, ResourceType type, int count, const Color4B& color)
{
    if(XButton::init(isBigSize ? XButton::BSize::Big : XButton::BSize::Small, XButton::BType::Blue)) {
        _isBigSize = isBigSize;
        _animated = animated;
        _color = color;
        
        addIconNode(type);
        const string& message = StringUtils::format("%d", count);
        const int fontSize = _isBigSize ? DEFAULT_FONT_SIZE : DEFAULT_FONT_SIZE;
        _countLabel = CocosUtils::createLabel(message, fontSize);
        _countLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        _countLabel->setTextColor(color);
        addChild(_countLabel);
        
        const auto& size(getContentSize());
        Node* icon = _animated ? _iconNode : _icon;
        icon->setPosition(Point(size.width / 4, size.height / 2));
        _countLabel->setPosition(Point(size.width * 3 / 4, size.height / 2));
        
        return true;
    }
    
    return false;
}

ResourceType ResourceButton::getType() const
{
    return _type;
}

int ResourceButton::getCount() const
{
    return _count;
}

bool ResourceButton::isResourceEnough() const
{
    return _resourceEnough;
}

bool ResourceButton::isEnabled() const
{
    return _enabled;
}

void ResourceButton::setType(ResourceType type)
{
    _type = type;
    
    addIconNode(type);
    
    resize();
}

void ResourceButton::setCount(int count)
{
    _count = count;
    
    if (_countLabel) {
        _countLabel->setString(StringUtils::format("%d", count));
    }
    resize();
}

void ResourceButton::setResourceEnough(bool enable)
{
    if (_countLabel && _enabled && _resourceEnough != enable) {
        _resourceEnough = enable;
        _countLabel->setTextColor(getEnabledColor());
    }
}

void ResourceButton::setEnabled(bool enabled)
{
    XButton::setEnabled(enabled);
    
    if (_countLabel) {
        if (enabled) {
            _countLabel->setTextColor(getEnabledColor());
        } else {
            static const Color4B disabledColor(Color4B::WHITE);
            _countLabel->setTextColor(disabledColor);
        }
    }
}

void ResourceButton::addIconNode(ResourceType type)
{
    if (type == ResourceType::MAX) {
        return;
    }
    
    if (_animated) {
        Point pos(-1, -1);
        if (_iconNode) {
            pos = _icon->getPosition();
            _iconNode->removeFromParent();
            _iconNode = nullptr;
        }
        
        const string& file(StringUtils::format("%d.csb", type));
        _iconNode = CocosUtils::playAnimation(file, 0, true);
        addChild(_iconNode);
        
        if (pos.x > 0) {
            _iconNode->setPosition(pos);
        }
    } else {
        string file;
        if (_isBigSize) {
            file = StringUtils::format("GameImages/resources/icon_%dS.png", type);
        } else {
            file = StringUtils::format("GameImages/resources/icon_%dS.png", type);
        }
        
        if (_icon) {
            _icon->setTexture(file);
        } else {
            _icon = Sprite::create(file);
            addChild(_icon);
        }
    }
}

const Color4B& ResourceButton::getEnabledColor() const
{
    return _resourceEnough ? _color : Color4B::RED;
}

void ResourceButton::resize()
{
    if (/* DISABLES CODE */ (false)) {
        const float buttonWidth = getContentSize().width;
        const float iconWidth = _icon->getContentSize().width;
        const float labelWidth = _countLabel->getContentSize().width;
        const float x = buttonWidth / 2 - labelWidth / 2;
        _icon->setPositionX(x);
        _countLabel->setPositionX(x + 5.0f + iconWidth / 2 + labelWidth * _countLabel->getAnchorPoint().x);
    }
}
