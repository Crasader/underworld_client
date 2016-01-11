//
//  TowerManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "TowerManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"
#include "TowerData.h"

using namespace std;
using namespace cocostudio;

TowerManager::TowerManager()
{
}

TowerManager::~TowerManager()
{
    Utils::clearMap(_towers);
}

void TowerManager::init(const rapidjson::Value& jsonDict)
{
    
}

void TowerManager::upgrade(int id)
{
    if (_towers.find(id) != _towers.end()) {
        TowerData* data = _towers.at(id);
        if (data) {
            data->setLevel(data->getLevel() + 1);
        }
    } else {
        assert(false);
    }
}

const TowerData* TowerManager::getTowerData(int id)
{
    if (_towers.find(id) != _towers.end()) {
        return _towers.at(id);
    }
    
    return nullptr;
}
