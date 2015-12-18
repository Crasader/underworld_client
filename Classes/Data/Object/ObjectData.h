//
//  ObjectData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ObjectData_h
#define ObjectData_h

#include "json/document.h"
#include "CocosGlobal.h"

class ObjectLocalData;

class ObjectData
{
public:
    ObjectData(const rapidjson::Value& jsonDict);
    virtual ~ObjectData();
    
    int getId() const;
    const ObjectLocalData* getLocalData() const;
    int getCount() const;
    
private:
    int _id;
    int _count;
};

#endif /* ObjectData_h */
