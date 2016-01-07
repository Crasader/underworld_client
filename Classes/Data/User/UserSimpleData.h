//
//  UserSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UserSimpleData_h
#define UserSimpleData_h

#include <iostream>
#include "CocosGlobal.h"
#include "json/document.h"

class UserSimpleData
{
public:
    UserSimpleData(const rapidjson::Value& jsonDict);
    virtual ~UserSimpleData();
    
    int getUserId() const;
    
private:
    int _userId;
};

#endif /* UserSimpleData_h */
