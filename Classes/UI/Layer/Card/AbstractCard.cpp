//
//  AbstractCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/24.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AbstractCard.h"

AbstractCard::AbstractCard() {}
AbstractCard::~AbstractCard() {}

bool AbstractCard::init()
{
    if (Widget::init())
    {
        setAnchorPoint(cocos2d::Point::ANCHOR_MIDDLE);
        return true;
    }
    
    return false;
}

const CardSimpleData* AbstractCard::getCardData() const
{
    return nullptr;
}

void AbstractCard::move(const cocos2d::Point& point, const std::function<void()>& callback)
{
    static const int zorder_top(1);
    static const int zorder_bottom(0);
    static const float duration(0.2f);
    static const int actionTag(818);
    if (getPosition() != point) {
        if (getActionByTag(actionTag)) {
            stopActionByTag(actionTag);
        }
        
        setLocalZOrder(zorder_top);
        auto action = cocos2d::Sequence::create(cocos2d::MoveTo::create(duration, point), cocos2d::CallFunc::create([=]() {
            setPosition(point);
            setLocalZOrder(zorder_bottom);
            if (callback) {
                callback();
            }
        }), nullptr);
        action->setTag(actionTag);
        runAction(action);
    } else if (callback) {
        callback();
    }
}
