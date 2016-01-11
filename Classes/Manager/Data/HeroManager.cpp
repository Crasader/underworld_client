//
//  HeroManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"
#include "HeroData.h"
#include "ArtifactData.h"

using namespace std;
using namespace cocostudio;

HeroManager::HeroManager()
{
}

HeroManager::~HeroManager()
{
    Utils::clearMap(_heroes);
}

void HeroManager::init(const rapidjson::Value& jsonDict)
{
    
}

void HeroManager::upgrade(int id)
{
    if (_heroes.find(id) != _heroes.end()) {
        HeroData* data = _heroes.at(id);
        if (data) {
            data->setLevel(data->getLevel() + 1);
        }
    } else {
        assert(false);
    }
}

void HeroManager::setArtifact(int id, int index, const ArtifactData* artifact)
{
    if (_heroes.find(id) != _heroes.end()) {
        HeroData* data = _heroes.at(id);
        if (data) {
            data->setArtifact(index, artifact);
        }
    } else {
        assert(false);
    }
}

void HeroManager::upgradeArtifact(int id, int index)
{
    if (_heroes.find(id) != _heroes.end()) {
        HeroData* data = _heroes.at(id);
        if (data) {
            const ArtifactData* artifact = data->getArtifactData(index);
            data->setArtifactLevel(index, artifact->getLevel() + 1);
        }
    } else {
        assert(false);
    }
}

const HeroData* HeroManager::getHeroData(int id)
{
    if (_heroes.find(id) != _heroes.end()) {
        return _heroes.at(id);
    }
    
    return nullptr;
}
