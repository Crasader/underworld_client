//
//  ArtifactUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ArtifactUpgradeData_h
#define ArtifactUpgradeData_h

#include "AbstractUpgradeData.h"
#include <map>

class AttributeData;
class RewardData;

class ArtifactUpgradeData : public AbstractUpgradeData
{
public:
    ArtifactUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~ArtifactUpgradeData();
    
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    const RewardData* getSoldReward(int type) const;
    
private:
    std::map<int, AttributeData *> _attributes;
    std::map<int, RewardData*> _soldRewards;
};

#endif /* ArtifactUpgradeData_h */
