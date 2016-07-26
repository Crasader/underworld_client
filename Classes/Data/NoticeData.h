//
//  NoticeData.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef NoticeData_h
#define NoticeData_h

#include <string>
#include "json/document.h"

class NoticeData
{
public:
    NoticeData(const rapidjson::Value& jsonDict);
    virtual ~NoticeData();
    
    int getId() const;
    long getTimeStamp() const;
    const std::string& getMessage() const;
    
private:
    int _id;
    long _timeStamp;
    std::string _message;
};

#endif /* NoticeData_h */
