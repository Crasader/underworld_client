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

static const auto& normalTextColor(Color3B::WHITE);
static const auto& touchedTextColor(Color3B::ORANGE);

TabButton::TabButton() {}

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
    if(ui::Button::init(normal, normal, selected)) {
        setTitleFontName(DEFAULT_FONT);
        setTitleFontSize(DEFAULT_FONT_SIZE);
        setTitleColor(normalTextColor);
        setTitleText(title);
        addClickEventListener(callback);
        
        return true;
    }
    
    return false;
}

bool TabButton::init(const string& title, const Button::ccWidgetClickCallback& callback)
{
    static const auto normal(CocosUtils::getResourcePath("button_fenye_1.png"));
    static const auto selected(CocosUtils::getResourcePath("button_fenye.png"));
    return init(title, normal, selected, callback);
}

void TabButton::setEnabled(bool enabled)
{
    ui::Button::setEnabled(enabled);
    setTitleColor(enabled ? normalTextColor : touchedTextColor);
}