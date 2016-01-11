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
#include <map>

class AttributeData;
class SoldierLocalData;
class SoldierUpgradeData;
class SoldierQualityData;
class SoldierTalentData;
class GearData;

class SoldierData : public AbstractData
{
public:
    SoldierData(const rapidjson::Value& jsonDict);
    virtual ~SoldierData();
    
    // getters
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
    const SoldierLocalData* getLocalData() const;
    const SoldierUpgradeData* getUpgradeData() const;
    const SoldierQualityData* getQualityData() const;
    const SoldierTalentData* getTalentData() const;
    const GearData* getGearData(int index) const;
    
    // setters
    void setLevel(int level);
    void setQuality(int quality);
    void setTalent(int talent);
    void setGear(int index, const GearData* gear);
    void setGearLevel(int index, int level);
    
private:
    void calculateTotalAttributes();
    
private:
    std::map<int, AttributeData*> _attributes;
    int _quality;
    int _talent;
    std::map<int, GearData*> _gears;
};

#endif /* SoldierData_h */
