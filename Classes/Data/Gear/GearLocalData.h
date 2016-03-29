//
//  GearLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearLocalData_h
#define GearLocalData_h

#include "AbstractLocalData.h"
#include <unordered_set>
#include "CocosGlobal.h"

class GearLocalData : public AbstractLocalData
{
public:
    GearLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearLocalData();
    
    int getSetId() const;
    GearType getType() const;
    GearQuality getQuality() const;
    int getMaxCount() const;
    const std::unordered_set<int>& getAttributes() const;
    const std::unordered_set<int>& getSubAttributes() const;
    
private:
    int _setId;
    GearType _type;
    GearQuality _quality;
    int _maxCount;
    std::unordered_set<int> _attributes;
    std::unordered_set<int> _subAttributes;
};

#endif /* GearLocalData_h */
