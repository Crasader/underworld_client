//
//  HeroData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroData_h
#define HeroData_h

#include "json/document.h"

class HeroLocalData;

class HeroData
{
public:
    HeroData(const rapidjson::Value& jsonDict);
    virtual ~HeroData();
    
    int getUniqueId() const;
    const HeroLocalData* getLocalData() const;
    
private:
    int _id;
    int _uniqueId;
};

#endif /* HeroData_h */
