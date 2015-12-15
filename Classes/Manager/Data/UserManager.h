//
//  UserManager.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UserManager_h
#define UserManager_h

#include "cocos2d.h"
#include "json/document.h"

class UserData;
class UserSimpleData;

class UserManager
{
public:
    UserManager();
    virtual ~UserManager();
    
    void init(const rapidjson::Value& jsonDict);
    
    const std::vector<UserSimpleData*>& getFriends() const;
    void addFriend(int userId);
    void removeFriend(int userId);
    
    const std::vector<UserSimpleData*>& getBlackList() const;
    void addToBlackList(int userId);
    void removeFromBlackList(int userId);
    
protected:
    CC_DISALLOW_COPY_AND_ASSIGN(UserManager);
    
private:
    std::vector<UserSimpleData*> _friends;
    std::vector<UserSimpleData*> _blackList;
};

#endif /* UserManager_h */
