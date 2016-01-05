//
//  TowerData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef TowerData_h
#define TowerData_h

#include "json/document.h"

class TowerLocalData;

class TowerData
{
public:
    TowerData(const rapidjson::Value& jsonDict);
    virtual ~TowerData();
    
    int getUniqueId() const;
    const TowerLocalData* getLocalData() const;
    
private:
    int _id;
    int _uniqueId;
};

#endif /* TowerData_h */
