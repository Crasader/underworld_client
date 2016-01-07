//
//  SoldierPieceData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/6.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierPieceData_h
#define SoldierPieceData_h

#include "AbstractPieceLocalData.h"

class SoldierPieceData : public AbstractPieceLocalData
{
public:
    SoldierPieceData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierPieceData();
};

#endif /* SoldierPieceData_h */
