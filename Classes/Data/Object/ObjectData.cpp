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
:_id(0)
,_count(0)
{
    _id = DICTOOL->getIntValue_json(jsonDict, "id");
    _count = DICTOOL->getIntValue_json(jsonDict, "count");
}

ObjectData::~ObjectData()
{
    
}

int ObjectData::getId() const
{
    return _id;
}

const ObjectLocalData* ObjectData::getLocalData() const
{
    return DataManager::getInstance()->getObjectData(_id);
}

int ObjectData::getCount() const
{
    return _count;
}