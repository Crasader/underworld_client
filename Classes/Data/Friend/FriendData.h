//
//  FriendData.h
//  Underworld_Client
//
//  Created by burst on 16/1/22.
//
//

#ifndef FriendData_h
#define FriendData_h

#include <stdio.h>
#include "CocosGlobal.h"
#include "json/document.h"


class FriendData
{
public:
    FriendData(const rapidjson::Value& jsonDict);
    virtual ~FriendData();
    
    int getUserId() const;
    int getIconId() const;
    const std::string& getUserName() const;
    int getLevel() const;
private:
    int _userId;
    int _iconId;
    std::string _userName;
    int _level;
};

#endif /* FriendData_h */
