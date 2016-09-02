//
//  RuneData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneData_h
#define RuneData_h

#include "AbstractData.h"

class RuneData : public AbstractData
{
public:
    RuneData(const rapidjson::Value& jsonDict);
    virtual ~RuneData();
    
    void update(const rapidjson::Value& jsonDict);
};

#endif /* RuneData_h */
