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
    UAConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~UAConfigData();
    
    const AnimationParameter* getAnimationParameter(UnderWorld::Core::Unit::Direction direction) const;
    
protected:
    void parse(AnimationParameter* ret, const std::vector<std::string>& directions, const std::vector<std::string>& scales, const std::vector<std::string>& speeds, int index);
    
private:
    std::map<UnderWorld::Core::Unit::Direction, AnimationParameter*> _data;
};

#endif /* UAConfigData_h */
