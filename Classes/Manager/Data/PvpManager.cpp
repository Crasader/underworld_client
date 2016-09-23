//
//  PvpManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpManager.h"
#include "JSonUtils.h"
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

PvpManager::PvpManager() {}

PvpManager::~PvpManager()
{
    for (auto data : _logs) {
        CC_SAFE_DELETE(data);
    }
}

#pragma mark - public
#pragma mark - network request
void PvpManager::fetchPvpLogs(const std::function<void()>& callback)
{
    for (int i = 0; i < 10; ++i) {
        rapidjson::Document document;
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        document.AddMember("uid", 1, allocator);
        document.AddMember("user", rapidjson::Value("shadow", allocator), allocator);
        document.AddMember("time", 213123123, allocator);
        document.AddMember("content", rapidjson::Value("this is a test message\nthis is a test message\nthis is a test message", allocator), allocator);
        
        auto data = new (nothrow) PvpLogData(document);
        _logs.push_back(data);
    }
    
    if (callback) {
        callback();
    }
}

#pragma mark - data
const vector<PvpLogData*>& PvpManager::getPvpLogs() const
{
    return _logs;
}
