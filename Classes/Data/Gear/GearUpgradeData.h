//
//  GearUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearUpgradeData_h
#define GearUpgradeData_h

#include "CocosGlobal.h"
#include <map>

namespace tinyxml2 { class XMLElement; }

class ResourceData;
class RewardData;

class GearUpgradeData
{
public:
    GearUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearUpgradeData();
    
    int getId() const;
    int level() const;
    int getUnlockedLevel() const;
    int getResourceCount(ResourceType type) const;
    const std::vector<RewardData*>& getSoldRewards() const;
    
private:
    int _id;
    int _level;
    int _unlockedLevel;
    std::map<ResourceType, ResourceData*> _cost;
    std::vector<RewardData*> _soldRewards;
};

#endif /* GearUpgradeData_h */
