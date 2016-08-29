//
//  ResourceButton.cpp
//  Underworld_Client
//
//  Created by Andy on 15/7/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ResourceButton.h"
#include "CocosUtils.h"
#include "UniversalButton.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

ResourceButton::ResourceButton()
:_animated(false)
,_enabled(true)
,_type(ResourceType::Gold)
,_count(INVALID_VALUE)
,_color(Color4B::WHITE)
,_icon(nullptr)
,_iconNode(nullptr)
,_countLabel(nullptr)
,_button(nullptr)
{
    
}

ResourceButton::~ResourceButton()
{
    removeAllChildren();
}

ResourceButton* ResourceButton::create(bool isBigSize, bool animated, ResourceType type, int count, const Color4B& color, const Widget::ccWidgetClickCallback& callback)
{
    ResourceButton *p = new (std::nothrow) ResourceButton();
    if(p && p->init(isBigSize, animated, type, count, color, callback))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool ResourceButton::init(bool isBigSize, bool animated, ResourceType type, int count, const Color4B& color, const Widget::ccWidgetClickCallback& callback)
{
    if(Node::init())
    {
        _isBigSize = isBigSize;
        _animated = animated;
        _color = color;
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto button = UniversalButton::create(isBigSize ? UniversalButton::BSize::Big : UniversalButton::BSize::Small, UniversalButton::BType::Blue, "");
        addChild(button);
        _button = button;
        
        const auto& size(button->getContentSize());
        setContentSize(size);
        button->setPosition(size.width / 2, size.height / 2);
        
        addIconNode(type);
        const string& message = StringUtils::format("%d", count);
        const int fontSize = _isBigSize ? DEFAULT_FONT_SIZE : DEFAULT_FONT_SIZE;
        _countLabel = CocosUtils::createLabel(message, fontSize);
        _countLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        _countLabel->setTextColor(color);
        button->addChild(_countLabel);
        
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

void ResourceButton::setType(ResourceType type)
{
    _type = type;
    
    addIconNode(type);
    
    resize();
}

void ResourceButton::setCount(int count)
{
    _count = count;
    
    if (_countLabel)
    {
        _countLabel->setString(StringUtils::format("%d", count));
    }
    resize();
}

void ResourceButton::setEnabled(bool enable)
{
    if (_enabled != enable) {
        _enabled = enable;
        
        const Color4B color = enable ? _color : Color4B::RED;
        _countLabel->setTextColor(color);
    }
}

void ResourceButton::setClickEventListener(const Button::ccWidgetClickCallback& callback)
{
    if (_button)
    {
        _button->setCallback([=](Ref *pSender) {
            SoundManager::getInstance()->playButtonSound();
            callback(pSender);
        });
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
        _button->addChild(_iconNode);
        
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
            _button->addChild(_icon);
        }
    }
}

void ResourceButton::resize()
{
    const float buttonWidth = _button->getContentSize().width;
    const float iconWidth = _icon->getContentSize().width;
    const float labelWidth = _countLabel->getContentSize().width;
    const float x = buttonWidth / 2 - labelWidth / 2;
    _icon->setPositionX(x);
    _countLabel->setPositionX(x + 5.0f + iconWidth / 2 + labelWidth * _countLabel->getAnchorPoint().x);
}