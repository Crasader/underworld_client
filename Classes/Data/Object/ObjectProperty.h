//
//  ObjectProperty.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ObjectProperty_h
#define ObjectProperty_h

#include "AbstractProperty.h"

class ObjectProperty : public AbstractProperty
{
public:
    explicit ObjectProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~ObjectProperty();
};

#endif /* ObjectProperty_h */
