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

// first: scale, second: speed
typedef std::pair<float, float> AnimationParameters;

class UAConfigData
{
public:
    UAConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~UAConfigData();
    
    void getAnimationParameters(UnderWorld::Core::Unit::Direction direction, float& scale, float& speed);
    
protected:
    void parse(AnimationParameters& params, const std::vector<std::string>& directions, const std::vector<std::string>& scales, const std::vector<std::string>& speeds, int index);
    
private:
    std::map<UnderWorld::Core::Unit::Direction, AnimationParameters> _data;
};

#endif /* UAConfigData_h */
