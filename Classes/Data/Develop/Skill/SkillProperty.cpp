//
//  SkillProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillProperty.h"
#include "tinyxml2/tinyxml2.h"

SkillProperty::SkillProperty(tinyxml2::XMLElement *xmlElement)
:AbstractProperty(xmlElement)
,_beUnlockedLevel(0)
{
    attribute2Int(xmlElement, "unlock", _beUnlockedLevel);
}

SkillProperty::~SkillProperty() {}

int SkillProperty::getBeUnlockedLevel() const
{
    return _beUnlockedLevel;
}
