//
//  ObjectLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ObjectLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

ObjectLocalData::ObjectLocalData(tinyxml2::XMLElement *xmlElement)
:_objectId(0)
{
    if (xmlElement) {
        
    }
}

ObjectLocalData::~ObjectLocalData()
{
    
}

int ObjectLocalData::getObjectId() const
{
    return _objectId;
}

const vector<RewardData *>& ObjectLocalData::getRewards() const
{
    return _rewards;
}

const string& ObjectLocalData::getName() const
{
    return _name;
}

const string& ObjectLocalData::getDescription() const
{
    return _description;
}