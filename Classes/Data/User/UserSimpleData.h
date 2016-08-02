//
//  UserSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UserSimpleData_h
#define UserSimpleData_h

#include <string>
#include <vector>
#include "json/document.h"

class CardSimpleData;

class UserSimpleData
{
public:
    UserSimpleData(const rapidjson::Value& jsonDict);
    virtual ~UserSimpleData();
    
    int getUid() const;
    const std::string& getUser() const;
    int getIcon() const;
    int getLevel() const;
    int getTrophy() const;
    const std::string& getGuild() const;
    const std::vector<CardSimpleData*>& getCards() const;
    
private:
    int _uid;
    std::string _user;
    int _icon;
    int _level;
    int _trophy;
    std::string _guild;
    std::vector<CardSimpleData*> _cards;
};

#endif /* UserSimpleData_h */
