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

namespace tinyxml2 { class XMLElement; }

class SoldierLocalData
{
public:
    SoldierLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierLocalData();
    
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    std::string _name;
    std::string _description;
};

#endif /* SoldierLocalData_h */
