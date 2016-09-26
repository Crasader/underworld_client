//
//  ObjectBriefData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ObjectBriefData.h"
#include "Utils.h"
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
        _id = Utils::stoi(result.at(0));
        _count = Utils::stoi(result.at(1));
    } else if (size == 3) {
        _percentage = Utils::stoi(result.at(0));
        _id = Utils::stoi(result.at(1));
        _count = Utils::stoi(result.at(2));
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