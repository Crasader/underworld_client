//
//  SoldierManager.h
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierManager_h
#define SoldierManager_h

#include "json/document.h"
#include <unordered_map>
#include "CocosGlobal.h"

class SoldierData;
class GearData;

class SoldierManager
{
public:
    SoldierManager();
    virtual ~SoldierManager();
    
    void init(const rapidjson::Value& jsonDict);
    void requestForDetail(int id);
    void upgrade(int id);
    void recast(int id);
    void setGear(int id, int index, const GearData* gear);
    void upgradeGear(int id, int index);
    void switchTalent(int id, int talent);
    
    const SoldierData* getSoldierData(int id);
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(SoldierManager);
    
private:
    std::unordered_map<int, SoldierData*> _soldiers;
};

#endif /* SoldierManager_h */
