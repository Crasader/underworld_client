//
//  ResourceManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ResourceManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"

using namespace std;
using namespace cocostudio;

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager() {}

void ResourceManager::init(const rapidjson::Value& jsonDict)
{
    
}

int ResourceManager::getResourceCount(ResourceType type) const
{
    if (_resources.find(type) != end(_resources)) {
        return _resources.at(type);
    }
    
    return 0;
}

void ResourceManager::updateGemInfo(const rapidjson::Value& jsonDict)
{
    static ResourceType type(ResourceType::Gem);
    static const char* key("gemCount");
    if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
        auto amount = DICTOOL->getIntValue_json(jsonDict, key);
        if (_resources.find(type) != end(_resources)) {
            _resources.at(type) = amount;
        } else {
            _resources.insert(make_pair(type, amount));
        }
    }
}

void ResourceManager::updateResources(const rapidjson::Value& jsonDict, const char* key)
{
    if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
        for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
            const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
            auto type = static_cast<ResourceType>(DICTOOL->getIntValue_json(value, "id"));
            auto amount = DICTOOL->getIntValue_json(value, "amount");
            if (_resources.find(type) != end(_resources)) {
                _resources.at(type) = amount;
            } else {
                _resources.insert(make_pair(type, amount));
            }
        }
    }
}
