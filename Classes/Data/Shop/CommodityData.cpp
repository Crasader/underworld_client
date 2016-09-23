//
//  CommodityData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CommodityData.h"
#include "JSonUtils.h"
#include "DataManager.h"

CommodityData::CommodityData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    update(jsonDict);
}

CommodityData::~CommodityData() {}

void CommodityData::update(const rapidjson::Value& jsonDict)
{
    AbstractData::update(jsonDict);
    do {
        static const char* key("resources");
        CC_BREAK_IF(!JSonUtils::isExist(jsonDict, key));
        for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
            const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
            auto type(JSonUtils::parse<ResourceType>(value, "id"));
            auto amount(JSonUtils::parse<int>(value, "amount"));
            auto iter(_price.find(type));
            if (iter != end(_price)) {
                iter->second = amount;
            } else {
                _price.insert(make_pair(type, amount));
            }
        }
    } while (false);
}

int CommodityData::getPrice(ResourceType type) const
{
    auto iter(_price.find(type));
    if (iter != end(_price)) {
        return iter->second;
    }
    
    return 0;
}

const std::map<ResourceType, int>& CommodityData::getPrice() const
{
    return _price;
}
