//
//  UserGuildData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/6.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UserGuildData_h
#define UserGuildData_h

#include "json/document.h"

class GuildData;

class UserGuildData
{
public:
    enum class Role {
        None,
        Member,
        Elder,
        Coleader,
        Leader,
    };
    
    explicit UserGuildData(const rapidjson::Value& jsonDict);
    virtual ~UserGuildData();
    
    void update(const rapidjson::Value& jsonDict);
    
    int getUid() const;
    const GuildData* getGuildData() const;
    Role getRole() const;
    int getDonateCountdown() const;
    
private:
    int _uid;
    GuildData* _guildData;
    Role _role;
    int _countdown;
};

#endif /* UserGuildData_h */
