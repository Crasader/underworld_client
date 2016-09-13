//
//  PvpLogData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PvpLogData_h
#define PvpLogData_h

#include <string>
#include <unordered_map>
#include "json/document.h"

class UserSimpleData;

class PvpLogData
{
public:
    explicit PvpLogData(const rapidjson::Value& jsonDict);
    virtual ~PvpLogData();
    
    bool isHomeWinner() const;
    int getTrophy() const;
    long getTime() const;
    int getTowerHpPercentage(bool isHome) const;
    const UserSimpleData* getUserData(bool isHome) const;
    
private:
    bool _isHomeWinner;
    int _trophy;
    std::unordered_map<bool, int> _towerHpPercentages;
    std::unordered_map<bool, UserSimpleData*> _userDatas;
};

#endif /* PvpLogData_h */
