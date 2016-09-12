//
//  TestConfiguration.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TestConfiguration.h"

using namespace std;

static TestConfiguration* s_pInstance(nullptr);
TestConfiguration* TestConfiguration::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) TestConfiguration();
    }
    
    return s_pInstance;
}

void TestConfiguration::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

TestConfiguration::TestConfiguration()
:
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
_pvr(true)
,_onlineChat(true)
#else
_pvr(false)
,_onlineChat(false)
#endif
{}

TestConfiguration::~TestConfiguration() {}

bool TestConfiguration::isPVREnabled() const
{
    return _pvr;
}

bool TestConfiguration::isOnlineChatEnabled() const
{
    return _onlineChat;
}

void TestConfiguration::setPVR(bool enabled)
{
    if (_pvr != enabled) {
        _pvr = enabled;
    }
}

void TestConfiguration::setOnlineChat(bool enabled)
{
    if (_onlineChat != enabled) {
        _onlineChat = enabled;
    }
}
