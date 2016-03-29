//
//  NetworkController.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "NetworkController.h"

NetworkController::NetworkController()
{
    
}

NetworkController::~NetworkController()
{
    
}

static NetworkController* s_pSharedInstance = nullptr;

NetworkController * NetworkController::getInstance()
{
    if (s_pSharedInstance == nullptr)
    {
        s_pSharedInstance = new (std::nothrow) NetworkController();
    }
    return s_pSharedInstance;
}

void NetworkController::purge()
{
    if (s_pSharedInstance)
    {
        delete s_pSharedInstance;
        s_pSharedInstance = nullptr;
    }
}

#pragma mark - public
void NetworkController::pushRequest(cocos2d::network::HttpRequest* request)
{
    _httpRequests.push_back(request);
}

cocos2d::network::HttpRequest* NetworkController::popRequest()
{
    auto first = _httpRequests.begin();
    _httpRequests.erase(first);
    return *first;
}

bool NetworkController::isEmpty() const
{
    return _httpRequests.empty();
}