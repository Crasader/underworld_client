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
#include "AnimationParameter.h"

using namespace std;

UAConfigData::UAConfigData(tinyxml2::XMLElement *xmlElement)
{
    if (xmlElement) {
        const char* direction = xmlElement->Attribute("direction");
        if (direction) {
            vector<string> v;
            Utils::split(v, direction, "_");
            
            vector<string> v1;
            {
                const char* data = xmlElement->Attribute("scale");
                if (data) {
                    Utils::split(v1, data, "_");
                }
            }
            
            vector<string> v2;
            {
                const char* data = xmlElement->Attribute("speed");
                if (data) {
                    Utils::split(v2, data, "_");
                }
            }
            
            auto size(v.size());
            if (1 == size) {
                auto ap = new (nothrow) AnimationParameter();
                parse(ap, v, v1, v2, 0);
                for (int i = 0; i < UnderWorld::Core::Unit::DIRECTION_COUNT; ++i) {
                    auto ud = static_cast<UnderWorld::Core::Unit::Direction>(i);
                    _data.insert(make_pair(ud, new (nothrow) AnimationParameter(*ap)));
                }
                CC_SAFE_FREE(ap);
            } else {
                auto cnt((size > UnderWorld::Core::Unit::DIRECTION_COUNT) ? UnderWorld::Core::Unit::DIRECTION_COUNT : size);
                for (int i = 0; i < cnt; ++i) {
                    auto ap = new (nothrow) AnimationParameter();
                    parse(ap, v, v1, v2, i);
                    auto ud = static_cast<UnderWorld::Core::Unit::Direction>(i);
                    _data.insert(make_pair(ud, ap));
                }
            }
        }
    }
}

UAConfigData::~UAConfigData()
{
    Utils::clearMap(_data);
}

const AnimationParameter* UAConfigData::getAnimationParameter(UnderWorld::Core::Unit::Direction direction) const
{
    if (_data.find(direction) != end(_data)) {
        return _data.at(direction);
    }
    
    return nullptr;
}

void UAConfigData::parse(AnimationParameter* ret, const vector<string>& directions, const vector<string>& scales, const vector<string>& speeds, int index)
{
    ret->scale = ret->speed = 1.0f;
    
    if (directions.size() > index) {
        const auto& string(directions.at(index));
        if (string.length() > 0) {
            auto has = atoi(string.c_str());
            if (has) {
                if (scales.size() > index) {
                    auto s = atof(scales.at(index).c_str());
                    ret->scale = (s > 0) ? s : 1.0f;
                }
                
                if (speeds.size() > index) {
                    auto s = atof(speeds.at(index).c_str());
                    ret->speed = (s > 0) ? s : 1.0f;
                }
            }
        } else {
            assert(false);
        }
    }
}