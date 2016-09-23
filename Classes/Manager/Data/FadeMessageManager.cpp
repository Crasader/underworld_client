//
//  FadeMessageManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FadeMessageManager.h"

using namespace std;

static FadeMessageManager* s_pInstance(nullptr);
FadeMessageManager* FadeMessageManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) FadeMessageManager();
    }
    
    return s_pInstance;
}

void FadeMessageManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

FadeMessageManager::FadeMessageManager() {}

FadeMessageManager::~FadeMessageManager()
{
    
}

void FadeMessageManager::show(const std::string& message)
{
    
}
