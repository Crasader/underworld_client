//
//  AttributeLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AttributeLocalData_h
#define AttributeLocalData_h

#include "AbstractLocalData.h"

class AttributeLocalData : public AbstractLocalData
{
public:
    AttributeLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~AttributeLocalData();
};

#endif /* AttributeLocalData_h */
