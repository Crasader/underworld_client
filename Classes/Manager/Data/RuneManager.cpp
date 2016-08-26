//
//  RuneManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneManager.h"
#include "cocostudio/CocoStudio.h"
#include "RuneData.h"
#include "RuneGroupData.h"

using namespace std;
using namespace cocostudio;

static RuneManager* s_pInstance(nullptr);
RuneManager* RuneManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) RuneManager();
    }
    
    return s_pInstance;
}

void RuneManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

RuneManager::RuneManager()
{
    
}

RuneManager::~RuneManager()
{
    for (int i = 0; i < _runeGroups.size(); ++i) {
        CC_SAFE_DELETE(_runeGroups.at(i));
    }
}

size_t RuneManager::getRuneGroupsCount() const
{
    return _runeGroups.size();
}

const RuneGroupData* RuneManager::getRuneGroup(size_t idx)
{
    if (_runeGroups.size() > idx) {
        return _runeGroups.at(idx);
    }
    
    return nullptr;
}
