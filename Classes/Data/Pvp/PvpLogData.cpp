//
//  PvpLogData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpLogData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "JSonUtils.h"
#include "UserSimpleData.h"

using namespace std;

PvpLogData::PvpLogData(const rapidjson::Value& jsonDict)
:_isHomeWinner(false)
,_trophy(0)
{
    
}

PvpLogData::~PvpLogData()
{
    for (auto iter = begin(_userDatas); iter != end(_userDatas); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
}

bool PvpLogData::isHomeWinner() const
{
    return _isHomeWinner;
}

int PvpLogData::getTrophy() const
{
    return _trophy;
}

int PvpLogData::getTowerHpPercentage(bool isHome) const
{
    if (_towerHpPercentages.find(isHome) != end(_towerHpPercentages)) {
        return _towerHpPercentages.at(isHome);
    }
    
    return 0;
}

const UserSimpleData* PvpLogData::getUserData(bool isHome) const
{
    if (_userDatas.find(isHome) != end(_userDatas)) {
        return _userDatas.at(isHome);
    }
    
    return nullptr;
}
