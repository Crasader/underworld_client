//
//  ChestData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ChestData_h
#define ChestData_h

#include "json/document.h"

class ChestData
{
public:
    ChestData(const rapidjson::Value& jsonDict);
    virtual ~ChestData();
    
    void update(const rapidjson::Value& jsonDict);
    
private:
    
};

#endif /* ChestData_h */
