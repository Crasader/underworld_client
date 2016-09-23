//
//  AbstractCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AbstractCard.h"
#include "DataManager.h"
#include "CocosUtils.h"
#include "CardConfigData.h"
#include "AbstractData.h"
#include "AbstractProperty.h"

using namespace std;

AbstractCard::AbstractCard()
:_icon(nullptr)
,_level(nullptr)
,_qualityBox(nullptr)
,_cardId(0)
,_data(nullptr) {}

AbstractCard::~AbstractCard()
{
    removeAllChildren();
}

bool AbstractCard::update(int cardId, const AbstractData* data)
{
    bool ret(false);
    if (cardId != _cardId) {
        if (data) {
            _cardId = data->getId();
        } else {
            _cardId = cardId;
        }
        
        updateProperty(DataManager::getInstance()->getProperty(cardId));
        
        ret = true;
    }
    
    updateData(data);
    
    return ret;
}

int AbstractCard::getCardId() const
{
    return _cardId;
}

const AbstractData* AbstractCard::getCardData() const
{
    return _data;
}

void AbstractCard::updateData(const AbstractData* data)
{
    _data = data;
    if (_level) {
//        _level->setString(data ? StringUtils::format("LV.%d", data->getLevel()) : "");
    }
}
