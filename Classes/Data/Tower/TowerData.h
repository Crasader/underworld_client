//
//  TowerData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef TowerData_h
#define TowerData_h

#include "AbstractData.h"

class TowerLocalData;

class TowerData : public AbstractData
{
public:
    TowerData(const rapidjson::Value& jsonDict);
    virtual ~TowerData();
    
    const TowerLocalData* getLocalData() const;
};

#endif /* TowerData_h */
