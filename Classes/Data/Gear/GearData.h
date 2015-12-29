//
//  GearData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearData_h
#define GearData_h

#include <map>
#include "json/document.h"

class AttributeData;

class GearData
{
public:
    GearData(const rapidjson::Value& jsonDict);
    virtual ~GearData();
    
    int getId() const;
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
private:
    int _id;
    std::map<int, AttributeData *> _attributes;
};

#endif /* GearData_h */
