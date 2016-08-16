//
//  CardSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CardSimpleData_h
#define CardSimpleData_h

#include <string>
#include "json/document.h"

class CardSimpleData
{
public:
    CardSimpleData(const rapidjson::Value& jsonDict);
    CardSimpleData(int idx);
    CardSimpleData(const CardSimpleData* instance);
    virtual ~CardSimpleData();
    
    int getIdx() const;
    const std::string& getName() const;
    int getLevel() const;
    
private:
    int _idx;
    std::string _name;
    int _level;
};

#endif /* CardSimpleData_h */
