//
//  TabButton.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/17.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "TabButton.h"
#include "CocosUtils.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

static const Color4B& normalTextColor(Color4B::WHITE);
static const Color4B& touchedTextColor(Color4B::ORANGE);

TabButton::TabButton()
:_titleLabel(nullptr)
,_button(nullptr)
{
    
}

TabButton::~TabButton()
{
    removeAllChildren();
}

TabButton* TabButton::create(const string& title, const Button::ccWidgetClickCallback& callback)
{
    TabButton *p = new (nothrow) TabButton();
    if(p && p->init(title, callback))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool TabButton::init(const string& title, const Button::ccWidgetClickCallback& callback)
{
    if(Node::init())
    {
        const auto normal("GameImages/public/button_white_1.png");
        const auto selected("GameImages/public/button_black_1.png");
        _button = Button::create(normal, selected, selected);
        _button->addClickEventListener(callback);
        addChild(_button);
        
        const auto& size = _button->getContentSize();
        _titleLabel = CocosUtils::createLabel(title, DEFAULT_FONT_SIZE);
        _titleLabel->setPosition(Point(size.width / 2, size.height / 2));
        _titleLabel->setTextColor(normalTextColor);
        _button->addChild(_titleLabel);
        
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        _button->setPosition(Point(size.width / 2, size.height / 2));
        
        return true;
    }
    
    return false;
}

void TabButton::setEnabled(bool enabled)
{
    if (_button) {
        _button->setEnabled(enabled);
        
        if (_titleLabel) {
            if (enabled) {
                _titleLabel->setTextColor(normalTextColor);
            } else {
                _titleLabel->setTextColor(touchedTextColor);
            }
        }
    }
}