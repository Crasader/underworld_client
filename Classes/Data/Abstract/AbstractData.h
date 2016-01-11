//
//  AbstractData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AbstractData_h
#define AbstractData_h

#include "json/document.h"

class AbstractData
{
public:
    AbstractData(const rapidjson::Value& jsonDict);
    AbstractData(const AbstractData& instance);
    virtual ~AbstractData() = 0;
    
    int getId() const;
    int getUniqueId() const;
    int getLevel() const;
    virtual void setLevel(int level);
    
private:
    int _id;
    int _uniqueId;
    int _level;
};

#endif /* AbstractData_h */
