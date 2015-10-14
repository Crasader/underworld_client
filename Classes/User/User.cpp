//
//  User.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "User.h"
#include "cocostudio/CocoStudio.h"

using namespace cocostudio;

User::User(const rapidjson::Document& jsonDict)
{
    // parse with the data from network
    _uid = DICTOOL->getIntValue_json(jsonDict, kUID);
    _token = DICTOOL->getStringValue_json(jsonDict, kAuth, "");
    _isNewUser = DICTOOL->getBooleanValue_json(jsonDict, kNew);
    
    init();
}

User::~User()
{
    
}

#pragma mark - public
unsigned int User::uid() const
{
    return _uid;
}

bool User::isNewUser() const
{
    return _isNewUser;
}

bool User::isGuest() const
{
    return _isGuest;
}

const string& User::token() const
{
    return _token;
}

const string& User::name() const
{
    return _name;
}

void User::parseGemInfo(const rapidjson::Value& root)
{
    
}

void User::parseResources(const rapidjson::Value& root, const char* key, bool silent)
{
    
}

void User::loadUserInfo(const string& deviceToken, const httpRequestCallback& success, const httpRequestCallback& invalidTokenCallback, const httpRequestCallback& otherErrorCallback)
{
    NetworkApi::loadUserInfo(deviceToken, [=](cocos2d::network::HttpClient* client, cocos2d::network::HttpResponse* response) {
        if (NetworkApi::isSuccessfulResponse(response))
        {
            rapidjson::Document jsonDict;
            NetworkApi::parseResponseData(response->getResponseData(), jsonDict);
            parseUserInfo(jsonDict);
            
            if (success)
            {
                success();
            }
        }
        else
        {
            const long code = response->getResponseCode();
            if (code == InvalidTokenErrorCode)
            {
                if (invalidTokenCallback)
                {
                    invalidTokenCallback();
                }
            }
            else
            {
                if (otherErrorCallback)
                {
                    otherErrorCallback();
                }
            }
        }
    });
}

void User::init()
{
    
}

void User::parseUserInfo(const rapidjson::Document& jsonDict)
{
    
}

void User::parseDebugUserInfo(const rapidjson::Document& jsonDict)
{
    
}