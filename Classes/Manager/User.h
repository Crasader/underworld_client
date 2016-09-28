//
//  User.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef User_h
#define User_h

#include "json/document.h"
#include "CocosGlobal.h"

// from login response
#define kUID    ("uid")
#define kAuth   ("auth")

class UserSimpleData;

class User
{
public:
    explicit User(const rapidjson::Value& jsonDict);
    virtual ~User();
    
    // getters
    bool isNewUser() const;
    bool isGuest() const;
    bool isInBattle() const;
    int getUid() const;
    const std::string& getToken() const;
    UserSimpleData* getUserData() const;
    
    bool isFirstLogin() const;
    void saveFirstLoginTag();
    
    bool hasFinishedTutorial(int idx) const;
    void saveFinishedTutorialTag(int idx);
    
    //------------------------------ NETWORKING BEGIN ------------------------------//
    void loadUserInfo(const std::string& deviceToken, const httpRequestCallback& success, const httpErrorCallback& onError);
    //------------------------------ NETWORKING END ------------------------------//
    
private:
    M_DISALLOW_COPY_AND_ASSIGN(User);
    void init();
    
    //------------------------------ NETWORKING BEGIN ------------------------------//
    void parseUserInfo(const rapidjson::Value& jsonDict);
    //------------------------------ NETWORKING END ------------------------------//
private:
    bool _isNewUser;
    bool _isGuest;
    bool _isInBattle;
    std::string _token;
    
    bool _isFirstLogin;
    int _finishedTutorialsMaxIdx;
    
    UserSimpleData* _userData;
};

#endif /* User_h */
