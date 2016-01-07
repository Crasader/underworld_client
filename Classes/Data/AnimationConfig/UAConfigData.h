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
#include <map>
#include <vector>

namespace tinyxml2 { class XMLElement; }

// =====================================================
// Animation Parameters
// =====================================================

struct AnimationParameters
{
    float scale;
    float speed;
};

// =====================================================
// Unit Animation Config Data
// =====================================================

class UAConfigData
{
public:
    UAConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~UAConfigData();
    
    AnimationParameters getAnimationParameters(UnitDirection direction);
    
protected:
    AnimationParameters parse(const std::vector<std::string>& directions, const std::vector<std::string>& scales, const std::vector<std::string>& speeds, int index);
    
private:
    std::map<UnitDirection, AnimationParameters> _data;
};

#endif /* UAConfigData_h */
