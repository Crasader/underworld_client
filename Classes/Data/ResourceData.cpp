//
//  ResourceData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ResourceData.h"
#include "Utils.h"

using namespace std;

ResourceData::ResourceData(const string& content)
:_id(ResourceType::Gem)
,_count(0)
{
    vector<string> result;
    Utils::split(result, content, "_", "");
    const size_t size = result.size();
    if (size == 2) {
        _id = static_cast<ResourceType>(atoi(result.at(0).c_str()));
        _count = atoi(result.at(1).c_str());
    } else {
        assert(false);
    }
}

ResourceData::~ResourceData()
{
    
}

ResourceType ResourceData::getId() const
{
    return _id;
}

int ResourceData::getCount() const
{
    return _count;
}
