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
#include "GameModeHMM.h"
#include "GameConstants.h"

using namespace std;
using namespace cocostudio;

CardSimpleData::CardSimpleData(const rapidjson::Value& jsonDict)
:_idx(0)
,_level(0)
,_cost(0)
{
    _idx = DICTOOL->getIntValue_json(jsonDict, "id");
    _level = DICTOOL->getIntValue_json(jsonDict, "level");
    
    auto ct = DataManager::getInstance()->getGameModeHMM()->findCardTypeById(_idx);
    if (ct) {
        _name = ct->getName();
    } else {
        _name = "";
    }
    
    auto iter = ct->getCost().find(RES_NAME_WOOD);
    _cost = (iter == ct->getCost().end()) ? 0 : UnderWorld::Core::GameConstants::microres2Res(iter->second);
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

const string& CardSimpleData::getName() const
{
    return _name;
}

int CardSimpleData::getLevel() const
{
    return _level;
}

int CardSimpleData::getCost() const
{
    return _cost;
}

bool CardSimpleData::isHero() const
{
    return _cost <= 0;
}
