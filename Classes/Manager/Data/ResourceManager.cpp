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

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    
}

void ResourceManager::init(const rapidjson::Value& jsonDict)
{
    
}

int ResourceManager::getResourceCount(ResourceType type) const
{
    if (_resources.find(type) != _resources.end())
    {
        return _resources.at(type);
    }
    
    return 0;
}

void ResourceManager::updateGemInfo(const rapidjson::Value& jsonDict)
{
    if (DICTOOL->checkObjectExist_json(jsonDict, "gemCount"))
    {
        _resources[kResourceType_Gem] = DICTOOL->getIntValue_json(jsonDict, "gemCount");
    }
}

void ResourceManager::updateResources(const rapidjson::Value& jsonDict, const char* key)
{
    if (DICTOOL->checkObjectExist_json(jsonDict, key))
    {
        const rapidjson::Value& resource = DICTOOL->getSubDictionary_json(jsonDict, key);
        for (int type = kResourceType_Gem; type < kResourceType_MAX; ++type)
        {
            const char *key = Utils::format("%d", type).c_str();
            if (DICTOOL->checkObjectExist_json(resource, key))
            {
                int count = DICTOOL->getIntValue_json(resource, key, 0);
                _resources[static_cast<ResourceType>(type)] = count;
            }
        }
    }
}
