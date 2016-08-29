//
//  RuneData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneData_h
#define RuneData_h

#include <string>
#include "json/document.h"

class RuneData
{
public:
    RuneData(const rapidjson::Value& jsonDict);
    virtual ~RuneData();
    
    int getDbIdx() const;
    int getIdx() const;
    int getLevel() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::string& getEffect() const;
    
protected:
    int _dbIdx;
    int _idx;
    int _level;
};

#endif /* RuneData_h */
