//
//  CheatConfiguration.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CheatConfiguration.h"

using namespace std;

static CheatConfiguration* s_pInstance(nullptr);
CheatConfiguration* CheatConfiguration::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) CheatConfiguration();
    }
    
    return s_pInstance;
}

void CheatConfiguration::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

CheatConfiguration::CheatConfiguration()
:loadPVR(false) {}

CheatConfiguration::~CheatConfiguration() {}
