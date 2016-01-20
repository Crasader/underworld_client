//
//  ChatData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatData_h
#define ChatData_h

#include <iostream>
#include "CocosGlobal.h"
#include "json/document.h"

class ChatData
{
public:
    ChatData(ChatType type, const rapidjson::Value& jsonDict);
    virtual ~ChatData();
    
    int getId() const;
    ChatType getType() const;
    long getTimeStamp() const;
    int getFromUserId() const;
    int getToUserId() const;
    const std::string& getFromUserName() const;
    const std::string& getToUserName() const;
    const std::string& getMessage() const;
    
private:
    int _id;
    ChatType _type;
    long _timeStamp;
    int _fromUserId;
    std::string _fromUserName;
    int _toUserId;
    std::string _toUserName;
    std::string _message;
};

#endif /* ChatData_h */
