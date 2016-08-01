//
//  PvpManager.h
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PvpManager_h
#define PvpManager_h

#include "json/document.h"
#include "CocosGlobal.h"
#include <vector>
#include <map>
#include <functional>

class PvpLogData;

class PvpManager
{
public:
    static PvpManager* getInstance();
    static void purge();
    
    void parse(const rapidjson::Value& jsonDict);
    const std::vector<PvpLogData*>& getPvpLogData() const;
    
private:
    PvpManager();
    virtual ~PvpManager();
    M_DISALLOW_COPY_AND_ASSIGN(PvpManager);
    
    void clear();
    
private:
    std::vector<PvpLogData*> _logData;
};

#endif /* PvpManager_h */
