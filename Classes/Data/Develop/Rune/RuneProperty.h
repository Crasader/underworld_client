//
//  RuneProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneProperty_h
#define RuneProperty_h

#include "AbstractProperty.h"

class RuneProperty : public AbstractProperty
{
public:
    explicit RuneProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~RuneProperty();
    
private:
};

#endif /* RuneProperty_h */
