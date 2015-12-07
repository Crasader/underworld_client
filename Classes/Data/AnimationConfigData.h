//
//  AnimationConfigData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AnimationConfigData_h
#define AnimationConfigData_h

#include <map>
#include "CocosGlobal.h"

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
// Animation Config Data
// =====================================================

class AnimationConfigData
{
public:
    AnimationConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~AnimationConfigData();
    
    AnimationParameters getAnimationParameters(UnitDirection direction);
    
private:
    std::map<UnitDirection, AnimationParameters> _data;
};

#endif /* AnimationConfigData_h */
