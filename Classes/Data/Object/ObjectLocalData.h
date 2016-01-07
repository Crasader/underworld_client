//
//  ObjectLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ObjectLocalData_h
#define ObjectLocalData_h

#include "AbstractLocalData.h"

class ObjectLocalData : public AbstractLocalData
{
public:
    ObjectLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~ObjectLocalData();
};

#endif /* ObjectLocalData_h */
