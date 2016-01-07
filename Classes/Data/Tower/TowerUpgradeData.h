//
//  TowerUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef TowerUpgradeData_h
#define TowerUpgradeData_h

#include "AbstractUpgradeData.h"

class TowerUpgradeData : public AbstractUpgradeData
{
public:
    TowerUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~TowerUpgradeData();
};

#endif /* TowerUpgradeData_h */
