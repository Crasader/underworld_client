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

class AbstractProperty;
class AbstractUpgradeProperty;

class AbstractData
{
public:
    static int getCardId(const AbstractData* data);
    explicit AbstractData(const rapidjson::Value& jsonDict);
    virtual ~AbstractData() = 0;
    
    void update(const rapidjson::Value& jsonDict);
    
    int getDbId() const;
    int getId() const;
    int getLevel() const;
    int getAmount() const;
    const AbstractProperty* getProperty() const;
    const AbstractUpgradeProperty* getUpgradeProperty() const;
    const AbstractUpgradeProperty* getNextLevelUpgradeProperty() const;
    
protected:
    int _dbId;
    int _id;
    int _level;
    int _amount;
};

#endif /* AbstractData_h */
