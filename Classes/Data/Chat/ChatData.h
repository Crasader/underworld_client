//
//  ChatData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatData_h
#define ChatData_h

#include <string>
#include <vector>
#include "CocosGlobal.h"
#include "json/document.h"

class ObjectBriefData;

class ChatData
{
public:
    explicit ChatData(const rapidjson::Value& jsonDict);
    virtual ~ChatData();
    
    int getId() const;
    ChatType getType() const;
    int getUid() const;
    const std::string& getUser() const;
    int getIcon() const;
    long getTime() const;
    const std::string& getFormattedTime() const;
    const std::string& getMessage() const;
    const std::vector<ObjectBriefData*>& getRewards() const;
    
private:
    int _id;
    ChatType _type;
    int _uid;
    std::string _user;
    int _icon;
    long _time;
    std::string _formattedTime;
    std::string _message;
    std::vector<ObjectBriefData*> _rewards;
};

#endif /* ChatData_h */
