//
//  AbstractData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AbstractData_h
#define AbstractData_h

#include "json/document.h"
#include <string>

class AbstractProperty;

class AbstractData
{
public:
    AbstractData(const rapidjson::Value& jsonDict);
    virtual ~AbstractData() = 0;
    
    void update(const rapidjson::Value& jsonDict);
    
    int getDbId() const;
    int getId() const;
    int getLevel() const;
    int getAmount() const;
    
    virtual const AbstractProperty* getProperty() const = 0;
    
    // TODO: remove test code
    virtual const std::string& getName() const = 0;
    virtual const std::string& getDescription() const = 0;
    
protected:
    void json2Int(const rapidjson::Value& jsonDict, const char* key, int& output);
    
protected:
    int _dbId;
    int _id;
    int _level;
    int _amount;
};

#endif /* AbstractData_h */
