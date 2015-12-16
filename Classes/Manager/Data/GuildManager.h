//
//  GuildManager.h
//  Underworld_Client
//
//  Created by Andy on 15/11/20.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GuildManager_h
#define GuildManager_h

#include "json/document.h"
#include "CocosGlobal.h"

class GuildManager
{
public:
    GuildManager();
    virtual ~GuildManager();
    
    void initGuild(const rapidjson::Value& jsonDict);
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(GuildManager);
};

#endif /* GuildManager_h */
