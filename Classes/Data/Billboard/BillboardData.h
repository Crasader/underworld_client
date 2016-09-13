//
//  BillboardData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BillboardData_h
#define BillboardData_h

#include "json/document.h"

class UserSimpleData;

class BillboardData
{
public:
    explicit BillboardData(const rapidjson::Value& jsonDict);
    virtual ~BillboardData();
    
    void update(const rapidjson::Value& jsonDict);
    
    int getRank() const;
    const UserSimpleData* getUserData() const;
    
private:
    int _rank;
    UserSimpleData* _userData;
};

#endif /* BillboardData_h */
