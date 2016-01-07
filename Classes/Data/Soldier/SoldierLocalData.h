//
//  SoldierLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierLocalData_h
#define SoldierLocalData_h

#include "AbstractLocalData.h"
#include <map>

class AttributeData;

class SoldierLocalData : public AbstractLocalData
{
public:
    SoldierLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierLocalData();
    
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
private:
    std::map<int, AttributeData *> _attributes;
};

#endif /* SoldierLocalData_h */
