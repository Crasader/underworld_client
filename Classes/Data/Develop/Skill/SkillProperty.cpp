//
//  SkillProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillProperty.h"
#include "XMLUtils.h"

SkillProperty::SkillProperty(tinyxml2::XMLElement *xmlElement)
:DevelopProperty(xmlElement)
,_beUnlockedLevel(0)
{
    XMLUtils::parse(xmlElement, "unlock", _beUnlockedLevel);
}

SkillProperty::~SkillProperty() {}

int SkillProperty::getBeUnlockedLevel() const
{
    return _beUnlockedLevel;
}
