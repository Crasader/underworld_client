//
//  HeroLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroLocalData_h
#define HeroLocalData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class HeroLocalData
{
public:
    HeroLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~HeroLocalData();
    
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    std::string _name;
    std::string _description;
};

#endif /* HeroLocalData_h */
