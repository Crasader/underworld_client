//
//  PvpManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpManager.h"
#include "cocostudio/CocoStudio.h"
#include "PvpLogData.h"

using namespace std;

static PvpManager* s_pInstance(nullptr);
PvpManager* PvpManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) PvpManager();
    }
    
    return s_pInstance;
}

void PvpManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

PvpManager::PvpManager()
{
    
}

PvpManager::~PvpManager()
{
    clear();
}

void PvpManager::parse(const rapidjson::Value& jsonDict)
{
    
}

const vector<PvpLogData*>& PvpManager::getPvpLogData() const
{
    return _logData;
}

void PvpManager::clear()
{
    for (auto data : _logData) {
        CC_SAFE_DELETE(data);
    }
    
    _logData.clear();
}
