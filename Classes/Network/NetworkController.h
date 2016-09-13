//
//  NetworkController.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef NetworkController_h
#define NetworkController_h

#include "network/HttpClient.h"
#include <vector>

class NetworkController
{
public:
    static NetworkController * getInstance();
    static void purge();
    
    void pushRequest(cocos2d::network::HttpRequest* request);
    void popRequest();
    bool isEmpty() const;
    
private:
    NetworkController();
    virtual ~NetworkController();
    
private:
    std::vector<cocos2d::network::HttpRequest*> _httpRequests;
};

#endif /* NetworkController_h */
