//
//  GearData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearData_h
#define GearData_h

#include "AbstractData.h"
#include <map>
#include "CocosGlobal.h"

class AttributeData;
class GearLocalData;
class GearUpgradeData;

class GearData : public AbstractData
{
public:
    GearData(const rapidjson::Value& jsonDict);
    virtual ~GearData();
    
    GearQuality getQuality() const;
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
    const GearLocalData* getLocalData() const;
    const GearUpgradeData* getUpgradeData() const;
    
private:
    std::map<int, AttributeData *> _attributes;
};

#endif /* GearData_h */
