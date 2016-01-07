//
//  AbstractUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AbstractUpgradeData_h
#define AbstractUpgradeData_h

#include "CocosGlobal.h"
#include <map>

namespace tinyxml2 { class XMLElement; }

class ResourceData;

class AbstractUpgradeData
{
public:
    AbstractUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~AbstractUpgradeData() = 0;
    
    int getId() const;
    int getLevel() const;
    int getUnlockedLevel() const;
    int getResourceCount(ResourceType type) const;
    
private:
    int _id;
    int _level;
    int _unlockedLevel;
    std::map<ResourceType, ResourceData*> _cost;
};

#endif /* AbstractUpgradeData_h */
