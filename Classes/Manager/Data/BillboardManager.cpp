//
//  BillboardManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BillboardManager.h"
#include "JSonUtils.h"
#include "BillboardData.h"

using namespace std;

static BillboardManager* s_pInstance(nullptr);
BillboardManager* BillboardManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) BillboardManager();
    }
    
    return s_pInstance;
}

void BillboardManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

BillboardManager::BillboardManager() {}

BillboardManager::~BillboardManager()
{
    for (int i = 0; i < _list.size(); ++i) {
        CC_SAFE_DELETE(_list.at(i));
    }
}

#pragma mark - public
#pragma mark - network request
void BillboardManager::getList(const function<void()>& callback)
{
    if (_list.empty()) {
        for (int i = 0; i < 200; ++i) {
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            document.AddMember("rank", i + 1, allocator);
            _list.push_back(new (nothrow) BillboardData(document));
        }
    }
    
    if (callback) {
        callback();
    }
}

#pragma mark - billboard
const vector<BillboardData*>& BillboardManager::getBillboard() const
{
    return _list;
}
