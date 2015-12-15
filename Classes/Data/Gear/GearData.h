//
//  GearData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearData_h
#define GearData_h

#include <vector>
#include "json/document.h"

class GearAttribute;

class GearData
{
public:
    GearData(const rapidjson::Value& jsonDict);
    virtual ~GearData();
    
    int getId() const;
    const std::vector<GearAttribute *>& getAttributes() const;
    
private:
    int _id;
    std::vector<GearAttribute *> _attributes;
};

#endif /* GearData_h */
