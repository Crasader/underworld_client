//
//  CardSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CardSimpleData_h
#define CardSimpleData_h

#include "AbstractData.h"

class CardSimpleData : public AbstractData
{
public:
    CardSimpleData(const rapidjson::Value& jsonDict);
    virtual ~CardSimpleData();
    
    bool operator==(const CardSimpleData& instance) const;
    
    void update(const rapidjson::Value& jsonDict);
};

#endif /* CardSimpleData_h */
