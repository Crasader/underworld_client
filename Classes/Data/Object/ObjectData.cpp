//
//  ObjectData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ObjectData.h"
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"
#include "ObjectLocalData.h"

using namespace cocostudio;

ObjectData::ObjectData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
,_count(0)
{
    _count = DICTOOL->getIntValue_json(jsonDict, "count");
}

ObjectData::~ObjectData()
{
    
}

const ObjectLocalData* ObjectData::getLocalData() const
{
    return DataManager::getInstance()->getObjectData(getId());
}

int ObjectData::getCount() const
{
    return _count;
}