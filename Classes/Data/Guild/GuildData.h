//
//  GuildData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/5.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildData_h
#define GuildData_h

#include "json/document.h"
#include <string>

class GuildData
{
public:
    enum class Type {
        Open,
        InviteOnly,
        Closed
    };
    
    explicit GuildData(const rapidjson::Value& jsonDict);
    virtual ~GuildData();
    
    void update(const rapidjson::Value& jsonDict);
    
    int getId() const;
    Type getType() const;
    int getBadge() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    int getMembersAmount() const;
    int getTotalTrophies() const;
    int getRequiredTrophies() const;
    int getRedEnvelopes() const;
    int getDonations() const;
    
private:
    int _id;
    Type _type;
    int _badge;
    std::string _name;
    std::string _description;
    int _membersAmount;
    int _totalTrophies;
    int _requiredTrophies;
    int _redEnvelopes;
    int _donations;
};

#endif /* GuildData_h */
