//
//  QuestData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestData_h
#define QuestData_h

#include "json/document.h"
#include "CocosGlobal.h"

class QuestProperty;

class QuestData
{
public:
    QuestData(const rapidjson::Value& jsonDict);
    virtual ~QuestData();
    
    QuestType getType() const;
    int getId() const;
    const QuestProperty* getProperty() const;
    int getProgress() const;
    int getStatus() const;
    
private:
    QuestType _type;
    int _id;
    int _progress;
    int _status;
};

#endif /* QuestData_h */
