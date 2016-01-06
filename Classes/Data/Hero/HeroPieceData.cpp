//
//  HeroPieceData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/6.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroPieceData.h"
#include "RewardData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "AttributeData.h"

using namespace std;

HeroPieceData::HeroPieceData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_requiredCount(0)
{
    if (xmlElement) {
        _id = atoi(xmlElement->Attribute("id"));
        _requiredCount = atoi(xmlElement->Attribute("require"));
        
        {
            const char *data = xmlElement->Attribute("name");
            if (data) {
                _name.assign(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("desc");
            if (data) {
                _description.assign(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("worth");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ";", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    RewardData* reward = new (nothrow) RewardData(*iter);
                    _soldRewards.push_back(reward);
                }
            }
        }
    }
}

HeroPieceData::~HeroPieceData()
{
    Utils::clearVector(_soldRewards);
}

int HeroPieceData::getId() const
{
    return _id;
}

int HeroPieceData::getRequiredCount() const
{
    return _requiredCount;
}

const string& HeroPieceData::getName() const
{
    return _name;
}

const string& HeroPieceData::getDescription() const
{
    return _description;
}

const vector<RewardData*>& HeroPieceData::getSoldRewards() const
{
    return _soldRewards;
}
