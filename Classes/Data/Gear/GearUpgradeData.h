//
//  GearUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearUpgradeData_h
#define GearUpgradeData_h

#include "AbstractUpgradeData.h"

class ObjectBriefData;

class GearUpgradeData : public AbstractUpgradeData
{
public:
    GearUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearUpgradeData();
    
    const ObjectBriefData* getSoldReward(int type) const;
    
private:
    std::unordered_map<int, ObjectBriefData*> _soldRewards;
};

#endif /* GearUpgradeData_h */
