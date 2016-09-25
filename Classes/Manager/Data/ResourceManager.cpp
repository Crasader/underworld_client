//
//  ResourceManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ResourceManager.h"
#include "JSonUtils.h"
#include "Utils.h"
#include "SkillBookData.h"

using namespace std;

static ResourceManager* s_pInstance(nullptr);
ResourceManager* ResourceManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) ResourceManager();
    }
    
    return s_pInstance;
}

void ResourceManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

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
    auto amount(JSonUtils::parse<int>(jsonDict, "gemCount"));
    static ResourceType type(ResourceType::Gem);
    if (_resources.find(type) != end(_resources)) {
        _resources.at(type) = amount;
    } else {
        _resources.insert(make_pair(type, amount));
    }
}

void ResourceManager::updateResources(const rapidjson::Value& jsonDict)
{
    do {
        static const char* key("resources");
        CC_BREAK_IF(!JSonUtils::isExist(jsonDict, key));
        for (int i = 0; i < DICTOOL->getArrayCount_json(jsonDict, key); ++i) {
            const auto& value = DICTOOL->getDictionaryFromArray_json(jsonDict, key, i);
            auto type(JSonUtils::parse<ResourceType>(value, "id"));
            auto amount(JSonUtils::parse<int>(value, "amount"));
            if (_resources.find(type) != end(_resources)) {
                _resources.at(type) = amount;
            } else {
                _resources.insert(make_pair(type, amount));
            }
        }
    } while (false);
    
    do {
        static const char* key("book");
        CC_BREAK_IF(!JSonUtils::isExist(jsonDict, key));
        const auto& value = DICTOOL->getSubDictionary_json(jsonDict, key);
        auto bookId(JSonUtils::parse<int>(value, "id"));
        if (_books.find(bookId) != end(_books)) {
            _books.at(bookId)->update(value);
        } else {
            _books.insert(make_pair(bookId, new (nothrow) SkillBookData(value)));
        }
    } while (false);
}
