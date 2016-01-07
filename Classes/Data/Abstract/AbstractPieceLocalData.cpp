//
//  AbstractPieceLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractPieceLocalData.h"
#include "RewardData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "AttributeData.h"

using namespace std;

AbstractPieceLocalData::AbstractPieceLocalData(tinyxml2::XMLElement *xmlElement)
:AbstractLocalData(xmlElement)
,_requiredCount(0)
{
    if (xmlElement) {
        _requiredCount = atoi(xmlElement->Attribute("require"));
        
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

AbstractPieceLocalData::~AbstractPieceLocalData()
{
    Utils::clearVector(_soldRewards);
}

int AbstractPieceLocalData::getRequiredCount() const
{
    return _requiredCount;
}

const vector<RewardData*>& AbstractPieceLocalData::getSoldRewards() const
{
    return _soldRewards;
}
