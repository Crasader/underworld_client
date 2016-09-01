//
//  RuneProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneProperty.h"
#include "tinyxml2/tinyxml2.h"

RuneProperty::RuneProperty(tinyxml2::XMLElement *xmlElement)
:AbstractProperty(xmlElement) {}

RuneProperty::~RuneProperty() {}
