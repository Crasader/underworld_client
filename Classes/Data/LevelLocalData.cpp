//
//  LevelLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "LevelLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

LevelLocalData::LevelLocalData(tinyxml2::XMLElement *xmlElement)
:_levelId(0)
{
    if (xmlElement) {
        
    }
}

LevelLocalData::~LevelLocalData()
{
    
}

int LevelLocalData::getLevelId() const
{
    return _levelId;
}

const map<ResourceType, int>& LevelLocalData::rewards() const
{
    return _rewards;
}

int LevelLocalData::getRewardCount(ResourceType type) const
{
    if (_rewards.find(type) != _rewards.end())
    {
        return _rewards.at(type);
    }
    
    return 0;
}