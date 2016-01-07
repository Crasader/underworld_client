//
//  ObjectData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ObjectData_h
#define ObjectData_h

#include "AbstractData.h"

class ObjectLocalData;

class ObjectData : public AbstractData
{
public:
    ObjectData(const rapidjson::Value& jsonDict);
    virtual ~ObjectData();
    
    const ObjectLocalData* getLocalData() const;
    int getCount() const;
    
private:
    int _count;
};

#endif /* ObjectData_h */
