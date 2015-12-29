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
#include <map>

namespace tinyxml2 { class XMLElement; }

class AttributeData;
class SkillLocalData;

class HeroLocalData
{
public:
    HeroLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~HeroLocalData();
    
    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    const SkillLocalData* getSkillData() const;
    
private:
    int _id;
    std::string _name;
    std::string _description;
    std::map<int, AttributeData *> _attributes;
    int _skillId;
};

#endif /* HeroLocalData_h */
