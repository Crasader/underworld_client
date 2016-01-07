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
#include <set>
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
    const std::set<int>& getAttributes() const;
    const std::set<int>& getSubAttributes() const;
    
private:
    int _setId;
    GearType _type;
    GearQuality _quality;
    int _maxCount;
    std::set<int> _attributes;
    std::set<int> _subAttributes;
};

#endif /* GearLocalData_h */
