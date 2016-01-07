//
//  SoldierData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierData_h
#define SoldierData_h

#include "AbstractData.h"

class SoldierLocalData;
class SoldierUpgradeData;
class SoldierQualityData;
class SoldierTalentData;

class SoldierData : public AbstractData
{
public:
    SoldierData(const rapidjson::Value& jsonDict);
    virtual ~SoldierData();
    
    const SoldierLocalData* getLocalData() const;
    const SoldierUpgradeData* getUpgradeData() const;
    const SoldierQualityData* getQualityData() const;
    const SoldierTalentData* getTalentData() const;
    
private:
    int _quality;
    int _talent;
};

#endif /* SoldierData_h */
