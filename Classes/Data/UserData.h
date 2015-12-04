//
//  UserData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UserData_h
#define UserData_h

#include <iostream>
#include "CocosGlobal.h"
#include "json/document.h"

// =====================================================
// User Data
// =====================================================

class UserData
{
public:
    UserData(const rapidjson::Value& jsonDict);
    virtual ~UserData();
    
    int getUserId() const;
    
private:
    int _userId;
};

#endif /* UserData_h */
