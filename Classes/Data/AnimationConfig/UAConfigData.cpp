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
using namespace UnderWorld::Core;

static const vector<Unit::Direction> AnimationDirections = {
    Unit::kDirection_Up,
    Unit::kDirection_leftUp2,
    Unit::kDirection_leftUp1,
    Unit::kDirection_left,
    Unit::kDirection_leftDown1,
    Unit::kDirection_leftDown2,
    Unit::kDirection_Down,
};

static const map<Unit::Direction, Unit::Direction> FlippedDirections = {
    {Unit::kDirection_rightUp2, Unit::kDirection_leftUp2},
    {Unit::kDirection_rightUp1, Unit::kDirection_leftUp1},
    {Unit::kDirection_right, Unit::kDirection_left},
    {Unit::kDirection_rightDown1, Unit::kDirection_leftDown1},
    {Unit::kDirection_rightDown2, Unit::kDirection_leftDown2},
};

UAConfigData::UAConfigData(tinyxml2::XMLElement *xmlElement)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("name");
        _skill = static_cast<SkillClass>(atoi(xmlElement->Attribute("skill")));
        parse(xmlElement, "scale");
        parse(xmlElement, "speed");
        parse(xmlElement, "atk_separater");
        parse(xmlElement, "cast_separater");
    }
}

UAConfigData::~UAConfigData()
{
    Utils::clearMap(_data);
}

const string& UAConfigData::getName() const
{
    return _name;
}

SkillClass UAConfigData::getSkill() const
{
    return _skill;
}

const AnimationParameter* UAConfigData::getAnimationParameter(Unit::Direction direction) const
{
    auto ret = get(direction);
    if (!ret) {
        if (FlippedDirections.find(direction) != end(FlippedDirections)) {
            ret = get(FlippedDirections.at(direction));
        }
        
        if (!ret && _data.size() > 0) {
            ret = getEnd();
        }
    }
    
    return ret;
}

void UAConfigData::parse(tinyxml2::XMLElement *element, const string& key)
{
    if (element) {
        auto data = element->Attribute(key.c_str());
        if (data) {
            vector<string> v;
            Utils::split(v, data, "_");
            for (int i = 0; i < v.size(); ++i) {
                CC_BREAK_IF(i >= AnimationDirections.size());
                const auto& direction = AnimationDirections.at(i);
                if (_data.find(direction) == end(_data)) {
                    _data.insert(make_pair(direction, new (nothrow) AnimationParameter(getEnd())));
                }
                
                auto ap = _data.at(direction);
                auto value = atof(v.at(i).c_str());
                if ("scale" == key) {
                    ap->scale = value;
                } else if ("speed" == key) {
                    ap->speed = value;
                } else if ("atk_separater" == key) {
                    ap->atkIdx = value;
                } else if ("cast_separater" == key) {
                    ap->castIdx = value;
                }
            }
        }
    }
}

const AnimationParameter* UAConfigData::get(const Unit::Direction& direction) const
{
    if (_data.find(direction) != end(_data)) {
        return _data.at(direction);
    }
    
    return nullptr;
}

const AnimationParameter* UAConfigData::getEnd() const
{
    auto cnt(_data.size());
    if (cnt > 0) {
        return _data.at(AnimationDirections[cnt - 1]);
    }
    
    return nullptr;
}
