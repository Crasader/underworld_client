//
//  AbstractProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AbstractProperty_h
#define AbstractProperty_h

#include <string>

namespace tinyxml2 { class XMLElement; }

class AbstractProperty
{
public:
    explicit AbstractProperty(tinyxml2::XMLElement* xmlElement);
    virtual ~AbstractProperty() = 0;
    
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
private:
    int _id;
    std::string _name;
    std::string _description;
};

#endif /* AbstractProperty_h */
