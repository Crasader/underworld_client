//
//  ContentData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ContentData.h"
#include "Utils.h"

using namespace std;

ContentData::ContentData(const string& content)
:_type(0)
,_id(0)
,_count(0)
{
    vector<string> result;
    Utils::split(result, content, "_", "");
    const size_t size = result.size();
    if (size == 3) {
        _type = atoi(result.at(0).c_str());
        _id = atoi(result.at(2).c_str());
        _count = atoi(result.at(1).c_str());
    } else {
        assert(false);
    }
}

ContentData::~ContentData()
{
    
}

int ContentData::getType() const
{
    return _type;
}

int ContentData::getId() const
{
    return _id;
}

int ContentData::getCount() const
{
    return _count;
}
