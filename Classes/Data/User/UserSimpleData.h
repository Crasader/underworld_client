//
//  UserSimpleData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UserSimpleData_h
#define UserSimpleData_h

#include "json/document.h"
#include <string>
#include <vector>

class UserArenaData;
class UserGuildData;
class CardData;

class UserSimpleData
{
public:
    UserSimpleData(const rapidjson::Value& jsonDict);
    virtual ~UserSimpleData();
    
    void update(const rapidjson::Value& jsonDict);
    
    int getUid() const;
    const std::string& getName() const;
    int getIcon() const;
    int getLevel() const;
    int getExp() const;
    const UserArenaData* getArenaData() const;
    const UserGuildData* getGuildData() const;
    const std::vector<CardData*>& getCards() const;
    
private:
    int _uid;
    std::string _name;
    int _icon;
    int _level;
    int _exp;
    UserArenaData* _arenaData;
    UserGuildData* _guildData;
    std::vector<CardData*> _cards;
};

#endif /* UserSimpleData_h */
