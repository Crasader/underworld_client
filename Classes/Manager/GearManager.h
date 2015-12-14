//
//  GearManager.h
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearManager_h
#define GearManager_h

#include "cocos2d.h"
#include "json/document.h"
#include "CocosGlobal.h"

class GearLocalData;

class GearManager
{
public:
    GearManager();
    virtual ~GearManager();
    
    void initGear(const rapidjson::Value& jsonDict);
    void upgradeGear();
    void recastGear();
    void sellGear();
    
protected:
    CC_DISALLOW_COPY_AND_ASSIGN(GearManager);
    
private:
    std::vector<GearLocalData*> _gears;
};

#endif /* GearManager_h */
