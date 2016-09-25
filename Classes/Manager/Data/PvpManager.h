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
#include <vector>
#include <functional>

class PvpLogData;

class PvpManager
{
public:
    static PvpManager* getInstance();
    static void purge();
    
    //------------------------- NETWORKING REQUEST -------------------------//
    void fetchPvpLogs(const std::function<void()>& callback);
    
    // data
    const std::vector<PvpLogData*>& getPvpLogs() const;
    
private:
    PvpManager();
    virtual ~PvpManager();
    PvpManager(const PvpManager &) = delete;
    PvpManager &operator =(const PvpManager &) = delete;
    
private:
    std::vector<PvpLogData*> _logs;
};

#endif /* PvpManager_h */
