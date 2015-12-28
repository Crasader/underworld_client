//
//  AttributeLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AttributeLocalData_h
#define AttributeLocalData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class AttributeLocalData
{
public:
    AttributeLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~AttributeLocalData();
    
    int getId() const;
    const std::string& getName() const;
    
private:
    int _id;
    std::string _name;
};

#endif /* AttributeLocalData_h */
