//
//  NetworkApi.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef NetworkApi_h
#define NetworkApi_h

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "json/document.h"

USING_NS_CC;
using namespace std;

#define TimeoutDuration     (10)

#define HttpSuccessCode         (200)
#define InvalidTokenErrorCode   (401)
#define VersionErrorCode        (888)
#define OfflineCode             (999)

namespace NetworkApi
{
    // basic
    void request(const string& path,
                 cocos2d::network::HttpRequest::Type type,
                 const map<string, string> * params,
                 const cocos2d::network::ccHttpRequestCallback& callback,
                 bool isLocalTest = false,
                 bool showLoadingView = true,
                 bool isImmediate = true,
                 int timeout = TimeoutDuration);
    
    inline bool isSuccessfulResponse(cocos2d::network::HttpResponse* response) { return (response->isSucceed() && response->getResponseCode() == HttpSuccessCode); }
    
    void parseResponseData(const vector<char>* responseData, rapidjson::Document& jsonDict);
    
    // http requests
    void login(const cocos2d::network::ccHttpRequestCallback& callback);
    
    void loadUserInfo(const string& deviceToken, const cocos2d::network::ccHttpRequestCallback& callback);
    
    void heartBeat(const cocos2d::network::ccHttpRequestCallback& callback, bool showLoadingView);
}

#endif /* NetworkApi_h */
