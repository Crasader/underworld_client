//
//  UAConfigData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UAConfigData_h
#define UAConfigData_h

#include "CocosGlobal.h"
#include "Unit.h"
#include <map>
#include <vector>

namespace tinyxml2 { class XMLElement; }

class AnimationParameter;

class UAConfigData
{
public:
    explicit UAConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~UAConfigData();
    
    const std::string& getName() const;
    UnderWorld::Core::SkillClass getSkill() const;
    const AnimationParameter* getAnimationParameter(UnderWorld::Core::Unit::Direction direction) const;
    
protected:
    void parse(tinyxml2::XMLElement *element, const std::string& key);
    const AnimationParameter* get(const UnderWorld::Core::Unit::Direction& direction) const;
    const AnimationParameter* getEnd() const;
    
private:
    std::string _name;
    UnderWorld::Core::SkillClass _skill;
    std::map<UnderWorld::Core::Unit::Direction, AnimationParameter*> _data;
};

#endif /* UAConfigData_h */
