//
//  GearSetLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearSetLocalData_h
#define GearSetLocalData_h

#include "AbstractLocalData.h"
#include <map>

class AttributeData;

class GearSetLocalData : public AbstractLocalData
{
public:
    GearSetLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearSetLocalData();
    
    const AttributeData* getAttribute(int gearCount) const;
    
private:
    std::map<int, AttributeData*> _attributes;
};

#endif /* GearSetLocalData_h */
