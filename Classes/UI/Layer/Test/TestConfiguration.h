//
//  TestConfiguration.h
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TestConfiguration_h
#define TestConfiguration_h

#include "cocos2d.h"

class TestConfiguration
{
public:
    static TestConfiguration* getInstance();
    static void purge();
    
    bool isPVREnabled() const;
    bool isOnlineChatEnabled() const;
    
    void setPVR(bool enabled);
    void setOnlineChat(bool enabled);
    
private:
    TestConfiguration();
    virtual ~TestConfiguration();
    CC_DISALLOW_COPY_AND_ASSIGN(TestConfiguration);
    
private:
    bool _pvr;
    bool _onlineChat;
};

#endif /* TestConfiguration_h */
