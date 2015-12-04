//
//  GearData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

#pragma mark =====================================================
#pragma mark Gear Data
#pragma mark =====================================================

GearData::GearData(tinyxml2::XMLElement *xmlElement)
:_type(static_cast<GearType>(-1))
,_quality(static_cast<GearQuality>(-1))
{
    if (xmlElement) {
        
    }
}

GearData::~GearData()
{
    
}

GearType GearData::getType() const
{
    return _type;
}

GearQuality GearData::getQuality() const
{
    return _quality;
}

const string&  GearData::getDescription() const
{
    return _description;
}