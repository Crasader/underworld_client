//
//  XButton.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/27.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "XButton.h"
#include "CocosUtils.h"

using namespace std;

static string getButtonFile(XButton::BSize size, XButton::BType type) {
    const bool big(XButton::BSize::Big == size);
    string file;
    switch (type) {
        case XButton::BType::Blue:
        {
            if (big) {
                file = "button_lanse.png";
            } else {
                file = "button_lanse_1.png";
            }
        }
            break;
        case XButton::BType::Red:
        {
            if (big) {
                file = "button_hongse_1.png";
            } else {
                file = "button_hongse_2.png";
            }
        }
            break;
        case XButton::BType::Green:
        {
            if (big) {
                file = "button_lvse_1.png";
            } else {
                file = "button_lvse_2.png";
            }
        }
            break;
        case XButton::BType::Purple:
        {
            if (big) {
                file = "button_zise.png";
            } else {
                file = "button_zise.png";
            }
        }
            
        default:
            break;
    }
    
    if (!file.empty()) {
        return CocosUtils::getResourcePath(file);
    }
    
    return "";
}

XButton::XButton()
:_bSize(BSize::Big)
,_bType(BType::Blue)
,_callback(nullptr)
,_touchInvalid(false) {}

XButton::~XButton()
{
    removeAllChildren();
}

XButton* XButton::create(BSize size, BType type)
{
    auto p = new (nothrow) XButton();
    if(p && p->init(size, type)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

XButton* XButton::createReturnButton(Node* parent, const Vec2& offset)
{
    if (parent) {
        auto button = XButton::create(XButton::BSize::Small, XButton::BType::Blue);
        parent->addChild(button);
        
        const auto& size(button->getContentSize());
        auto icon = Sprite::create(CocosUtils::getResourcePath("icon_jiantou.png"));
        icon->setPosition(Point(size.width / 2, size.height / 2));
        button->addChild(icon);
        
        const auto& pSize(parent->getContentSize());
        button->setPosition(Point(offset.x + size.width / 2, pSize.height - (offset.y + size.height / 2)));
        
        return button;
    }
    
    return nullptr;
}

bool XButton::init(BSize size, BType type)
{
    _bSize = size;
    _bType = type;
    
    const auto& file = getButtonFile(size, type);
    const string disabled = CocosUtils::getResourcePath((XButton::BType::Blue == type) ? "button_huise.png" : "button_huise.png");
    
    if (!file.empty() && ui::Button::init(file, file, disabled)) {
        setTitleFontName(DEFAULT_FONT);
        setTitleFontSize(DEFAULT_FONT_SIZE);
        setTitleColor(Color3B::WHITE);
        return true;
    }
    
    return false;
}

void XButton::setType(BType type)
{
    if (_bType != type) {
        _bType = type;
        
        const auto& file = getButtonFile(_bSize, type);
        loadTextures(file, file);
    }
}

void XButton::setCallback(const Callback& callback)
{
    _callback = callback;
    CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, _callback);
}
