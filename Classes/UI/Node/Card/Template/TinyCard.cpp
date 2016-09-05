//
//  TinyCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TinyCard.h"
#include "DataManager.h"
#include "CocosUtils.h"
#include "CardConfigData.h"
#include "AbstractData.h"
#include "AbstractProperty.h"

using namespace std;

TinyCard* TinyCard::create()
{
    auto ret = new (nothrow) TinyCard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

TinyCard::TinyCard()
:_callback(nullptr)
,_touchInvalid(false) {}

TinyCard::~TinyCard()
{
    removeAllChildren();
}

bool TinyCard::init()
{
    if (AbstractCard::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(Size(Width, Height));
        
        auto card = Sprite::create(CocosUtils::getResourcePath("icon_kapai.png"));
        card->setPosition(Point(Width / 2, Height / 2));
        addChild(card);
        _icon = card;
        
        setTouchEnabled(true);
        setSwallowTouches(false);
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, [this](Ref*) {
            if (_callback) {
                _callback();
            }
        });
        
        return true;
    }
    
    return false;
}

void TinyCard::updateProperty(const AbstractProperty* property)
{
#if false
    if (_icon && property) {
        auto cd(DataManager::getInstance()->getCardConfigData(property->getId()));
        if (cd) {
            _icon->setTexture(cd->getIcon());
        } else {
            _icon->initWithTexture(nullptr, Rect::ZERO);
        }
    }
    
    if (false) {
        _qualityBox->setTexture(CocosUtils::getResourcePath("ui_quality_box_blue.png"));
    }
#endif
}

void TinyCard::updateData(const AbstractData* data)
{
    AbstractCard::updateData(data);
}
