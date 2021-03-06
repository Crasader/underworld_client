//
//  ResourceManager.h
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ResourceManager_h
#define ResourceManager_h

#include "json/document.h"
#include <map>
#include <unordered_map>
#include "CocosGlobal.h"

class SkillBookData;

class ResourceManager
{
public:
    static ResourceManager* getInstance();
    static void purge();
    
    void init(const rapidjson::Value& jsonDict);
    int getResourceCount(ResourceType type) const;
    void updateGemInfo(const rapidjson::Value& jsonDict);
    void updateResources(const rapidjson::Value& jsonDict);
    
protected:
    ResourceManager();
    virtual ~ResourceManager();
    M_DISALLOW_COPY_AND_ASSIGN(ResourceManager);
    
private:
    std::map<ResourceType, int> _resources;
    std::unordered_map<int, SkillBookData*> _books;
};

#endif /* ResourceManager_h */
