//
//  ArtifactUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ArtifactUpgradeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ResourceData.h"
#include "AttributeData.h"
#include "RewardData.h"

using namespace std;

ArtifactUpgradeData::ArtifactUpgradeData(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeData(xmlElement)
{
    if (xmlElement)
    {
        {
            const char *data = xmlElement->Attribute("attr");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    AttributeData* attr = new (nothrow) AttributeData(*iter);
                    _attributes.insert(make_pair(attr->getId(), attr));
                }
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

ArtifactUpgradeData::~ArtifactUpgradeData()
{
    Utils::clearMap(_attributes);
}

const map<int, AttributeData *>& ArtifactUpgradeData::getAttributes() const
{
    return _attributes;
}

const AttributeData* ArtifactUpgradeData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const vector<RewardData*>& ArtifactUpgradeData::getSoldRewards() const
{
    return _soldRewards;
}
