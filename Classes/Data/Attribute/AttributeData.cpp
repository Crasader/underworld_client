//
//  AttributeData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AttributeData.h"
#include "Utils.h"
#include "DataManager.h"
#include "AttributeLocalData.h"

using namespace std;

AttributeData::AttributeData(const string& content)
:_id(0)
,_count(0)
{
    vector<string> result;
    Utils::split(result, content, "_", "");
    const size_t size = result.size();
    if (size == 2) {
        _id = atoi(result.at(0).c_str());
        _count = atoi(result.at(1).c_str());
    } else {
        assert(false);
    }
}

AttributeData::~AttributeData()
{
    
}

int AttributeData::getId() const
{
    return _id;
}

const AttributeLocalData* AttributeData::getLocalData() const
{
    return DataManager::getInstance()->getAttributeData(_id);
}

int AttributeData::getCount() const
{
    return _count;
}
