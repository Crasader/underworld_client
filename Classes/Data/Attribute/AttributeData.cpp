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

void AttributeData::calculateAttributes(const unordered_map<int, AttributeData *>& attributes, unordered_map<int, int>& output)
{
    for (auto iter = attributes.begin(); iter != attributes.end(); ++iter) {
        const int attrId = iter->first;
        const int count = iter->second->getCount();
        if (output.find(attrId) != output.end()) {
            const int value = output.at(attrId);
            output.at(attrId) = value + count;
        } else {
            output.insert(make_pair(attrId, count));
        }
    }
}

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

AttributeData::AttributeData(int id, int count)
:_id(id)
,_count(count)
{
    
}

AttributeData::AttributeData(const AttributeData& instance)
:_id(instance._id)
,_count(instance._count)
{
    
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
