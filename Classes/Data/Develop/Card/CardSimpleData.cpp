//
//  CardSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CardSimpleData.h"
#include "cocostudio/CocoStudio.h"

using namespace cocostudio;

CardSimpleData::CardSimpleData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
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
}
