//
//  ConditionData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ConditionData.h"
#include "Utils.h"
#include <cassert>

using namespace std;

ConditionData::ConditionData(const string& content)
:_id(0)
,_count(0)
{
    vector<string> result;
    Utils::split(result, content, "_", "");
    const size_t size = result.size();
    if (size == 2) {
        _id = stoi(result.at(0));
        _count = stoi(result.at(1));
    } else {
        assert(false);
    }
}

ConditionData::~ConditionData() {}

int ConditionData::getId() const
{
    return _id;
}

int ConditionData::getCount() const
{
    return _count;
}
