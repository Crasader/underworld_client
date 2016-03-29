//
//  TowerManager.h
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef TowerManager_h
#define TowerManager_h

#include "json/document.h"
#include <unordered_map>
#include "CocosGlobal.h"

class TowerData;

class TowerManager
{
public:
    TowerManager();
    virtual ~TowerManager();
    
    void init(const rapidjson::Value& jsonDict);
    void upgrade(int id);
    
    const TowerData* getTowerData(int id);
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(TowerManager);
    
private:
    std::unordered_map<int, TowerData*> _towers;
};

#endif /* TowerManager_h */
