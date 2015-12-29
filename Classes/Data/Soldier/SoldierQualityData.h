//
//  SoldierQualityData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierQualityData_h
#define SoldierQualityData_h

#include "CocosGlobal.h"
#include <map>

namespace tinyxml2 { class XMLElement; }

class ResourceData;
class AttributeData;

class SoldierQualityData
{
public:
    SoldierQualityData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierQualityData();
    
    int getId() const;
    int level() const;
    int getResourceCount(ResourceType type) const;
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    int getOutput() const;
    
private:
    int _id;
    int _level;
    std::map<ResourceType, ResourceData*> _cost;
    std::map<int, AttributeData *> _attributes;
    int _output;
};

#endif /* SoldierQualityData_h */
