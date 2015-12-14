//
//  GearLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "RewardData.h"

using namespace std;

#pragma mark =====================================================
#pragma mark Gear Data
#pragma mark =====================================================

GearLocalData::GearLocalData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_type(static_cast<GearType>(-1))
,_quality(static_cast<GearQuality>(-1))
,_maxCount(0)
,_extraAttribute(0)
{
    if (xmlElement) {
        const char *gearId = xmlElement->Attribute("id");
        _id = atoi(gearId);
        _type = static_cast<GearType>(gearId[0] - '0');
        {
            const char *data = xmlElement->Attribute("desc");
            if (data) {
                _name.assign(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("max");
            if (data) {
                _maxCount = atoi(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("quality");
            if (data) {
                _quality = static_cast<GearQuality>(atoi(data));
            }
        }
        {
            const char *data = xmlElement->Attribute("extra");
            if (data) {
                _extraAttribute = atoi(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("worth");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ";", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    RewardData* reward = new RewardData(*iter);
                    _soldRewards.push_back(reward);
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("info");
            if (data) {
                _description.assign(data);
            }
        }
    }
}

GearLocalData::~GearLocalData()
{
    for (int i = 0; i < _soldRewards.size(); ++i) {
        CC_SAFE_DELETE(_soldRewards.at(i));
    }
}

int GearLocalData::getId() const
{
    return _id;
}

GearType GearLocalData::getType() const
{
    return _type;
}

GearQuality GearLocalData::getQuality() const
{
    return _quality;
}

int GearLocalData::getMaxCount() const
{
    return _maxCount;
}

int GearLocalData::getExtraAttribute() const
{
    return _extraAttribute;
}

const vector<RewardData*>& GearLocalData::getSoldRewards() const
{
    return _soldRewards;
}

const string& GearLocalData::getName() const
{
    return _name;
}

const string& GearLocalData::getDescription() const
{
    return _description;
}