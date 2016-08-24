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

int AbstractCard::getCardId() const
{
    return 0;
}
