//
//  HeroPieceData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/6.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroPieceData_h
#define HeroPieceData_h

#include "AbstractPieceLocalData.h"

class HeroPieceData : public AbstractPieceLocalData
{
public:
    HeroPieceData(tinyxml2::XMLElement *xmlElement);
    virtual ~HeroPieceData();
};

#endif /* HeroPieceData_h */
