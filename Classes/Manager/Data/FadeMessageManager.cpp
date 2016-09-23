//
//  FadeMessageManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FadeMessageManager.h"

using namespace std;

static ShopManager* s_pInstance(nullptr);
ShopManager* ShopManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) ShopManager();
    }
    
    return s_pInstance;
}

void ShopManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

ShopManager::ShopManager() {}

ShopManager::~ShopManager()
{
    
}

void ShopManager::show(const std::string& message)
{
    
}
