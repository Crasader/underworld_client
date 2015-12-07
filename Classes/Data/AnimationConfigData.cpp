//
//  AnimationConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AnimationConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

#pragma mark =====================================================
#pragma mark Animation Config Data
#pragma mark =====================================================

AnimationConfigData::AnimationConfigData(tinyxml2::XMLElement *xmlElement)
{
    if (xmlElement) {
        const char* direction = xmlElement->Attribute("direction");
        if (direction) {
            vector<string> v;
            Utils::split(v, direction, "_");
            
            vector<string> v1;
            const char* scale = xmlElement->Attribute("scale");
            if (scale) {
                Utils::split(v1, scale, "_");
            }
            
            vector<string> v2;
            const char* speed = xmlElement->Attribute("speed");
            if (speed) {
                Utils::split(v2, speed, "_");
            }
            
            for (int i = 0; i < v.size(); ++i) {
                bool has = atoi(v.at(i).c_str());
                UnitDirection ud = static_cast<UnitDirection>(i + 2);
                AnimationParameters params;
                if (has) {
                    // TODO:
                } else {
                    params.scale = 1.0f;
                    params.speed = 1.0f;
                }
                
                _data.insert(make_pair(ud, params));
            }
        }
    }
}

AnimationConfigData::~AnimationConfigData()
{
    
}

AnimationParameters AnimationConfigData::getAnimationParameters(UnitDirection direction)
{
    if (_data.find(direction) != _data.end()) {
        return _data.at(direction);
    }
    
    return {1.0f, 1.0f};
}
