//
//  SoldierData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierData_h
#define SoldierData_h

#include "json/document.h"

class SoldierLocalData;

class SoldierData
{
public:
    SoldierData(const rapidjson::Value& jsonDict);
    virtual ~SoldierData();
    
    int getUniqueId() const;
    const SoldierLocalData* getLocalData() const;
    
private:
    int _id;
    int _uniqueId;
};

#endif /* SoldierData_h */
