//
//  TowerUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef TowerUpgradeData_h
#define TowerUpgradeData_h

#include "CocosGlobal.h"
#include <map>

namespace tinyxml2 { class XMLElement; }

class ResourceData;

class TowerUpgradeData
{
public:
    TowerUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~TowerUpgradeData();
    
    int getId() const;
    int level() const;
    int getResourceCount(ResourceType type) const;
    
private:
    int _id;
    int _level;
    std::map<ResourceType, ResourceData*> _cost;
};

#endif /* TowerUpgradeData_h */
