//
//  GearSetLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearSetLocalData_h
#define GearSetLocalData_h

#include <iostream>
#include <map>

namespace tinyxml2 { class XMLElement; }

class AttributeData;

class GearSetLocalData
{
public:
    GearSetLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearSetLocalData();
    
    int getId() const;
    const std::string& getName() const;
    const AttributeData* getAttribute(int gearCount) const;
    
private:
    int _id;
    std::string _name;
    std::map<int, AttributeData*> _attributes;
};

#endif /* GearSetLocalData_h */
