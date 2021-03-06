//
//  RuneGroupData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneGroupData_h
#define RuneGroupData_h

#include "RuneData.h"

class RuneGroupData : public RuneData
{
public:
    explicit RuneGroupData(const rapidjson::Value& jsonDict);
    virtual ~RuneGroupData();
    
    void update(const rapidjson::Value& jsonDict);
};

#endif /* RuneGroupData_h */
