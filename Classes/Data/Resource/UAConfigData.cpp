//
//  UAConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UAConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

#pragma mark =====================================================
#pragma mark Animation Config Data
#pragma mark =====================================================

UAConfigData::UAConfigData(tinyxml2::XMLElement *xmlElement)
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
            
            const size_t size(v.size());
            if (1 == v.size()) {
                AnimationParameters params = parse(v, v1, v2, 0);
                if (params.scale != 1.0f || params.speed != 1.0f) {
                    for (int i = 0; i < UNIT_DIRECTIONS_COUNT; ++i) {
                        UnitDirection ud = static_cast<UnitDirection>(i + 2);
                        _data.insert(make_pair(ud, params));
                    }
                }
            } else {
                const size_t cnt((size > UNIT_DIRECTIONS_COUNT) ? UNIT_DIRECTIONS_COUNT : size);
                for (int i = 0; i < cnt; ++i) {
                    AnimationParameters params = parse(v, v1, v2, i);
                    if (params.scale != 1.0f || params.speed != 1.0f) {
                        UnitDirection ud = static_cast<UnitDirection>(i + 2);
                        _data.insert(make_pair(ud, params));
                    }
                }
            }
        }
    }
}

UAConfigData::~UAConfigData()
{
    
}

AnimationParameters UAConfigData::getAnimationParameters(UnitDirection direction)
{
    if (_data.find(direction) != _data.end()) {
        return _data.at(direction);
    }
    
    return {1.0f, 1.0f};
}

AnimationParameters UAConfigData::parse(const vector<string>& directions, const vector<string>& scales, const vector<string>& speeds, int index)
{
    AnimationParameters params {1.0f, 1.0f};
    
    if (directions.size() > index) {
        const string& string(directions.at(index));
        if (string.length() > 0) {
            const bool has = atoi(string.c_str());
            if (has) {
                if (scales.size() > index) {
                    const float scale = atof(scales.at(index).c_str());
                    params.scale = (scale > 0) ? scale : 1.0f;
                } else {
                    params.scale = 1.0f;
                }
                
                if (speeds.size() > index) {
                    const float speed = atof(speeds.at(index).c_str());
                    params.speed = (speed > 0) ? speed : 1.0f;
                } else {
                    params.speed = 1.0f;
                }
            }
        } else {
            assert(false);
        }
    }
    
    return params;
}