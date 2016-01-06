//
//  ArtifactUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ArtifactUpgradeData_h
#define ArtifactUpgradeData_h

#include "CocosGlobal.h"
#include <map>

namespace tinyxml2 { class XMLElement; }

class ResourceData;
class AttributeData;
class RewardData;

class ArtifactUpgradeData
{
public:
    ArtifactUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~ArtifactUpgradeData();
    
    int getId() const;
    int level() const;
    int getUnlockedLevel() const;
    int getResourceCount(ResourceType type) const;
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    const std::vector<RewardData*>& getSoldRewards() const;
    
private:
    int _id;
    int _level;
    int _unlockedLevel;
    std::map<ResourceType, ResourceData*> _cost;
    std::map<int, AttributeData *> _attributes;
    std::vector<RewardData*> _soldRewards;
};

#endif /* ArtifactUpgradeData_h */
