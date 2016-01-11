//
//  GearManager.h
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearManager_h
#define GearManager_h

#include "json/document.h"
#include <vector>
#include "CocosGlobal.h"

class GearData;

class GearManager
{
public:
    GearManager();
    virtual ~GearManager();
    
    void init(const rapidjson::Value& jsonDict);
    void resolve(int index);
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(GearManager);
    
private:
    std::vector<GearData*> _gears;
};

#endif /* GearManager_h */
