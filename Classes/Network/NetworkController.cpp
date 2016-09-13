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

void NetworkController::popRequest()
{
    if (!isEmpty()) {
        _httpRequests.erase(_httpRequests.begin());
    }
}

bool NetworkController::isEmpty() const
{
    return _httpRequests.empty();
}