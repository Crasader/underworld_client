//
//  TowerLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef TowerLocalData_h
#define TowerLocalData_h

#include "AbstractLocalData.h"

class TowerLocalData : public AbstractLocalData
{
public:
    TowerLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~TowerLocalData();
};

#endif /* TowerLocalData_h */
