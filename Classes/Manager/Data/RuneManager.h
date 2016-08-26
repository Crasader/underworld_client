//
//  RuneManager.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneManager_h
#define RuneManager_h

#include "json/document.h"
#include <vector>

class RuneData;
class RuneGroupData;

class RuneManager
{
public:
    static RuneManager* getInstance();
    static void purge();
    
    size_t getRuneGroupsCount() const;
    const RuneGroupData* getRuneGroup(size_t idx);
    
private:
    RuneManager();
    virtual ~RuneManager();
    RuneManager(const RuneManager &) = delete;
    RuneManager &operator =(const RuneManager &) = delete;
    
private:
    std::vector<RuneGroupData*> _runeGroups;
};

#endif /* RuneManager_h */
