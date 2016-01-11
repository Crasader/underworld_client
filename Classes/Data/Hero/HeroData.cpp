//
//  HeroData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroData.h"
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"
#include "AttributeData.h"
#include "HeroLocalData.h"
#include "HeroUpgradeData.h"
#include "ArtifactData.h"

using namespace std;
using namespace cocostudio;

HeroData::HeroData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    
}

HeroData::~HeroData()
{
    Utils::clearMap(_attributes);
    Utils::clearMap(_artifacts);
}

const HeroLocalData* HeroData::getLocalData() const
{
    return DataManager::getInstance()->getHeroData(getId());
}

const HeroUpgradeData* HeroData::getUpgradeData() const
{
    return DataManager::getInstance()->getHeroUpgradeData(getId(), getLevel());
}

const ArtifactData* HeroData::getArtifactData(int index) const
{
    if (_artifacts.find(index) != _artifacts.end()) {
        return _artifacts.at(index);
    }
    
    return nullptr;
}

void HeroData::setLevel(int level)
{
    setLevel(level);
    calculateTotalAttributes();
}

void HeroData::setArtifact(int index, const ArtifactData* artifact)
{
    if (_artifacts.find(index) != _artifacts.end()) {
        CC_SAFE_DELETE(_artifacts.at(index));
        _artifacts.at(index) = new (nothrow) ArtifactData(*artifact);
        calculateTotalAttributes();
    } else {
        assert(false);
    }
}

void HeroData::setArtifactLevel(int index, int level)
{
    if (_artifacts.find(index) != _artifacts.end()) {
        ArtifactData* data = _artifacts.at(index);
        data->setLevel(level);
        calculateTotalAttributes();
    } else {
        assert(false);
    }
}

#pragma mark - private
void HeroData::calculateTotalAttributes()
{
    if (_attributes.size() > 0) {
        Utils::clearMap(_attributes);
    }
    
    map<int, int> attrs;
    AttributeData::calculateAttributes(getLocalData()->getAttributes(), attrs);
    AttributeData::calculateAttributes(getUpgradeData()->getAttributes(), attrs);
    for (map<int, ArtifactData*>::const_iterator iter = _artifacts.begin(); iter != _artifacts.end(); ++iter) {
        AttributeData::calculateAttributes(iter->second->getAttributes(), attrs);
    }
    for (map<int, int>::const_iterator iter = attrs.begin(); iter != attrs.end(); ++iter) {
        const int attrId = iter->first;
        AttributeData* attr = new (nothrow) AttributeData(attrId, iter->second);
        _attributes.insert(make_pair(attrId, attr));
    }
}
