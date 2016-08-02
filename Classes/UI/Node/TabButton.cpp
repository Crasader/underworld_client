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

static const auto& normalTextColor(Color3B::WHITE);
static const auto& touchedTextColor(Color3B::ORANGE);

TabButton::TabButton()
:_button(nullptr) {}

TabButton::~TabButton()
{
    removeAllChildren();
}

TabButton* TabButton::create(const string& title, const string& normal, const string& selected, const Button::ccWidgetClickCallback& callback)
{
    auto p = new (nothrow) TabButton();
    if(p && p->init(title, normal, selected, callback)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

TabButton* TabButton::create(const string& title, const Button::ccWidgetClickCallback& callback)
{
    auto p = new (nothrow) TabButton();
    if(p && p->init(title, callback)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool TabButton::init(const string& title, const string& normal, const string& selected, const Button::ccWidgetClickCallback& callback)
{
    if(Node::init())
    {
        _button = Button::create(normal, normal, selected);
        _button->addClickEventListener(callback);
        addChild(_button);
        
        _button->setTitleFontName(DEFAULT_FONT);
        _button->setTitleFontSize(DEFAULT_FONT_SIZE);
        _button->setTitleColor(normalTextColor);
        _button->setTitleText(title);
        
        const auto& size = _button->getContentSize();
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        _button->setPosition(Point(size.width / 2, size.height / 2));
        
        return true;
    }
    
    return false;
}

bool TabButton::init(const string& title, const Button::ccWidgetClickCallback& callback)
{
    static const auto normal(CocosUtils::getResourcePath("button_white_1.png"));
    static const auto selected(CocosUtils::getResourcePath("button_black_1.png"));
    return init(title, normal, selected, callback);
}

void TabButton::setEnabled(bool enabled)
{
    if (_button) {
        _button->setEnabled(enabled);
        
        if (enabled) {
            _button->setTitleColor(normalTextColor);
        } else {
            _button->setTitleColor(touchedTextColor);
        }
    }
}