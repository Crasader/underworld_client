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

using namespace std;
using namespace cocostudio;

CardSimpleData::CardSimpleData(const rapidjson::Value& jsonDict)
:_idx(0)
,_level(0)
,_amount(0)
,_cardType(nullptr)
{
    _idx = DICTOOL->getIntValue_json(jsonDict, "id");
    _level = DICTOOL->getIntValue_json(jsonDict, "level");
    _amount = DICTOOL->getIntValue_json(jsonDict, "amount");
    _cardType = DataManager::getInstance()->getGameModeHMM()->findCardTypeById(_idx);
}

CardSimpleData::~CardSimpleData() {}

bool CardSimpleData::operator==(const CardSimpleData& c) const
{
    return ((*this) == c || _idx == c._idx);
}

int CardSimpleData::getIdx() const
{
    return _idx;
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

int CardSimpleData::getLevel() const
{
    return _level;
}

int CardSimpleData::getAmount() const
{
    return _amount;
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

static const string emptyString("");

const string& CardSimpleData::getName() const
{
    if (_cardType) {
        return _cardType->getName();
    }
    
    return emptyString;
}

const string& CardSimpleData::getDescription() const
{
    if (_cardType) {
        
    }
    
    return emptyString;
}

const string& CardSimpleData::getUnlockInfo() const
{
    if (_cardType) {
        
    }
    
    return emptyString;
}
