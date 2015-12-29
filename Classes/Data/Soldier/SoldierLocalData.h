//
//  SoldierLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierLocalData_h
#define SoldierLocalData_h

#include <iostream>
#include <map>

namespace tinyxml2 { class XMLElement; }

class AttributeData;

class SoldierLocalData
{
public:
    SoldierLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierLocalData();
    
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
private:
    int _id;
    std::string _name;
    std::string _description;
    std::map<int, AttributeData *> _attributes;
};

#endif /* SoldierLocalData_h */
