//
//  GearLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearLocalData_h
#define GearLocalData_h

#include <iostream>
#include <vector>
#include <set>
#include "CocosGlobal.h"

namespace tinyxml2 { class XMLElement; }

class RewardData;

class GearLocalData
{
public:
    GearLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearLocalData();
    
    int getId() const;
    int getSetId() const;
    GearType getType() const;
    GearQuality getQuality() const;
    int getMaxCount() const;
    const std::set<int>& getAttributes() const;
    const std::set<int>& getSubAttributes() const;
    const std::vector<RewardData*>& getSoldRewards() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    int _setId;
    GearType _type;
    GearQuality _quality;
    int _maxCount;
    std::set<int> _attributes;
    std::set<int> _subAttributes;
    std::vector<RewardData*> _soldRewards;
    std::string _name;
    std::string _description;
};

#endif /* GearLocalData_h */
