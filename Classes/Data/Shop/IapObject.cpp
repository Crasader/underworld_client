//
//  IapObject.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "IapObject.h"

using namespace std;

IapObject::~IapObject()
{
    
}

void IapObject::setIndex(int index)
{
    _index = index;
}

int IapObject::index() const
{
    return _index;
}

int IapObject::objectId() const
{
    return _objectId;
}

int IapObject::count() const
{
    return _count;
}

float IapObject::price() const
{
    return _price;
}

const string& IapObject::title() const
{
    return _title;
}

const string& IapObject::description() const
{
    return _description;
}

const string& IapObject::priceString() const
{
    return _priceString;
}