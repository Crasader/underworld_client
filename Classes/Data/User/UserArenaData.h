//
//  UserArenaData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/6.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UserArenaData_h
#define UserArenaData_h

#include "json/document.h"
#include <string>

class UserArenaData
{
public:
    UserArenaData(const rapidjson::Value& jsonDict);
    virtual ~UserArenaData();
    
    void update(const rapidjson::Value& jsonDict);
    
    int getUid() const;
    int getTrophy() const;
    
private:
    int _uid;
    int _trophy;
};

#endif /* UserArenaData_h */
