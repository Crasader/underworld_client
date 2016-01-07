//
//  HeroData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroData_h
#define HeroData_h

#include "AbstractData.h"

class HeroLocalData;
class HeroUpgradeData;

class HeroData : public AbstractData
{
public:
    HeroData(const rapidjson::Value& jsonDict);
    virtual ~HeroData();
    
    const HeroLocalData* getLocalData() const;
    const HeroUpgradeData* getUpgradeData() const;
};

#endif /* HeroData_h */
