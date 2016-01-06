//
//  HeroUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroUpgradeData_h
#define HeroUpgradeData_h

#include "CocosGlobal.h"
#include <map>

namespace tinyxml2 { class XMLElement; }

class ResourceData;
class AttributeData;

class HeroUpgradeData
{
public:
    HeroUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~HeroUpgradeData();
    
    int getId() const;
    int level() const;
    int getUnlockedLevel() const;
    int getResourceCount(ResourceType type) const;
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
private:
    int _id;
    int _level;
    int _unlockedLevel;
    std::map<ResourceType, ResourceData*> _cost;
    std::map<int, AttributeData *> _attributes;
};

#endif /* HeroUpgradeData_h */
