//
//  AbstractLocalData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AbstractLocalData_h
#define AbstractLocalData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class AbstractLocalData
{
public:
    AbstractLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~AbstractLocalData() = 0;
    
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    std::string _name;
    std::string _description;
};

#endif /* AbstractLocalData_h */
