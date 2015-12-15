//
//  GearAttribute.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearAttribute.h"
#include "Utils.h"

using namespace std;

GearAttribute::GearAttribute(const string& content)
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

GearAttribute::~GearAttribute()
{
    
}

int GearAttribute::getId() const
{
    return _id;
}

int GearAttribute::getCount() const
{
    return _count;
}
