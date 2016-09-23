//
//  CommodityData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CommodityData_h
#define CommodityData_h

#include "AbstractData.h"
#include "CocosGlobal.h"
#include <map>

class CommodityData : public AbstractData
{
public:
    explicit CommodityData(const rapidjson::Value& jsonDict);
    virtual ~ CommodityData();
    
    void update(const rapidjson::Value& jsonDict);
    
    int getPrice(ResourceType type) const;
    const std::map<ResourceType, int>& getPrice() const;
    
protected:
    std::map<ResourceType, int> _price;
};

#endif /* CommodityData_h */
