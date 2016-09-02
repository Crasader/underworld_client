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
    static std::string EmptyString;
    
    AbstractProperty(tinyxml2::XMLElement* xmlElement);
    virtual ~AbstractProperty() = 0;
    
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    
protected:
    void attribute2Int(tinyxml2::XMLElement* xmlElement, const char* key, int& output) const;
    void attribute2String(tinyxml2::XMLElement* xmlElement, const char* key, std::string& output) const;
    
private:
    int _id;
    std::string _name;
    std::string _description;
};

#endif /* AbstractProperty_h */
