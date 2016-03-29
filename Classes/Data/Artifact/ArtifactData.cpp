//
//  ArtifactData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ArtifactData.h"
#include "Utils.h"
#include "AttributeData.h"
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"
#include "GearLocalData.h"
#include "GearUpgradeData.h"

using namespace std;

ArtifactData::ArtifactData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    
}

ArtifactData::ArtifactData(const ArtifactData& instance)
:AbstractData(instance)
{
    const auto& attrs = instance.getAttributes();
    for (auto iter = attrs.begin(); iter != attrs.end(); ++iter) {
        const AttributeData* data = iter->second;
        _attributes.insert(make_pair(iter->first, new (nothrow) AttributeData(*data)));
    }
}

ArtifactData::~ArtifactData()
{
    Utils::clearMap(_attributes);
}

const unordered_map<int, AttributeData *>& ArtifactData::getAttributes() const
{
    return _attributes;
}

const AttributeData* ArtifactData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const ArtifactLocalData* ArtifactData::getLocalData() const
{
    return DataManager::getInstance()->getArtifactData(getId());
}

const ArtifactUpgradeData* ArtifactData::getUpgradeData() const
{
    return DataManager::getInstance()->getArtifactUpgradeData(getId(), getLevel());
}
