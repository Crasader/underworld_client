//
//  EffectConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "EffectConfigData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

EffectConfigData::EffectConfigData(tinyxml2::XMLElement *xmlElement)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("effect_name");
    }
}

EffectConfigData::~EffectConfigData()
{
    
}

const string& EffectConfigData::getName() const
{
    return _name;
}
