//
//  SkillBookProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillBookProperty.h"
#include "XMLUtils.h"

SkillBookProperty::SkillBookProperty(tinyxml2::XMLElement *xmlElement)
:AbstractProperty(xmlElement)
,_beUnlockedLevel(0)
{
    XMLUtils::parse(xmlElement, "unlock", _beUnlockedLevel);
}

SkillBookProperty::~SkillBookProperty() {}

int SkillBookProperty::getBeUnlockedLevel() const
{
    return _beUnlockedLevel;
}
