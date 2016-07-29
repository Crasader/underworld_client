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

class AttributeData;
class ObjectBriefData;

class ArtifactUpgradeData : public AbstractUpgradeData
{
public:
    ArtifactUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~ArtifactUpgradeData();
    
    const std::unordered_map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    const ObjectBriefData* getSoldReward(int type) const;
    
private:
    std::unordered_map<int, AttributeData *> _attributes;
    std::unordered_map<int, ObjectBriefData*> _soldRewards;
};

#endif /* ArtifactUpgradeData_h */
