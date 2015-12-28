//
//  SkillLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SkillLocalData_h
#define SkillLocalData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class SkillLocalData
{
public:
    SkillLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~SkillLocalData();
    
    int getId() const;
    const std::string& getName() const;
    
private:
    int _id;
    std::string _name;
};

#endif /* SkillLocalData_h */
