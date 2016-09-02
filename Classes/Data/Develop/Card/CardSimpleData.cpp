//
//  CardSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CardSimpleData.h"
#include "cocostudio/CocoStudio.h"
#include "DataManager.h"
#include "GameConstants.h"
#include "AbstractProperty.h"

using namespace std;
using namespace cocostudio;

CardSimpleData::CardSimpleData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
,_cardType(nullptr)
{
    update(jsonDict);
}

CardSimpleData::~CardSimpleData() {}

bool CardSimpleData::operator==(const CardSimpleData& c) const
{
    return ((*this) == c || _id == c._id);
}

void CardSimpleData::update(const rapidjson::Value& jsonDict)
{
    AbstractData::update(jsonDict);
    _cardType = DataManager::getInstance()->getGameModeHMM()->findCardTypeById(_id);
}

UnderWorld::Core::HMMCardClass CardSimpleData::getCardClass() const
{
    if (_cardType) {
        return _cardType->getCardClass();
    }
    
    return UnderWorld::Core::HMMCardClass::HHMCARD_CLASS_COUNT;
}

bool CardSimpleData::isHero() const
{
    return UnderWorld::Core::HMMCardClass::kHMMCardClass_Hero == getCardClass();
}

int CardSimpleData::getCost() const
{
    if (_cardType) {
        const auto& costs(_cardType->getCost());
        auto iter(costs.find(RES_NAME_WOOD));
        if (iter != end(costs)) {
            return UnderWorld::Core::GameConstants::microres2Res(iter->second);
        }
    }
    
    return 0;
}

int CardSimpleData::getQuality() const
{
    if (_cardType) {
        
    }
    
    return 0;
}

const string& CardSimpleData::getName() const
{
    if (_cardType) {
        return _cardType->getName();
    }
    
    return AbstractProperty::EmptyString;
}
