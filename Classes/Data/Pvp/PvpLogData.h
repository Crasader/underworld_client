//
//  PvpLogData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PvpLogData_h
#define PvpLogData_h

#include <string>
#include <vector>
#include "json/document.h"

class PvpLogData
{
public:
    PvpLogData(const rapidjson::Value& jsonDict);
    virtual ~PvpLogData();
    
    const std::string& getHomeUser() const;
    const std::string& getAwayUser() const;
    
private:
    int _homeUid;
    std::string _homeUser;
    int _awayUid;
    std::string _awayUser;
};

#endif /* PvpLogData_h */
