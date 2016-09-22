//
//  ObjectBriefData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ObjectBriefData.h"
#include "Utils.h"
#include <cstdlib>
#include <cassert>

using namespace std;

ObjectBriefData::ObjectBriefData(const string& content)
:_percentage(0)
,_id(0)
,_count(0)
{
    vector<string> result;
    Utils::split(result, content, "_", "");
    const size_t size = result.size();
    if (size == 2) {
        _percentage = 100;
        _id = atoi(result.at(0).c_str());
        _count = atoi(result.at(1).c_str());
    } else if (size == 3) {
        _percentage = atoi(result.at(0).c_str());
        _id = atoi(result.at(1).c_str());
        _count = atoi(result.at(2).c_str());
    } else {
        assert(false);
    }
}

ObjectBriefData::~ObjectBriefData() {}

int ObjectBriefData::getPercentage() const
{
    return _percentage;
}

int ObjectBriefData::getId() const
{
    return _id;
}

int ObjectBriefData::getCount() const
{
    return _count;
}