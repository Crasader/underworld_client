//
//  MapUnitConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/26.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUnitConfigData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

static const string suffix(".csb");

MapUnitConfigData::MapUnitConfigData(tinyxml2::XMLElement *xmlElement)
:_mapId(0)
,_isWerewolf(false)
{
    if (xmlElement) {
        {
            const char *data = xmlElement->Attribute("mapId");
            if (data && strlen(data) > 0) {
                _mapId = atoi(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("is_werewolf");
            if (data && strlen(data) > 0) {
                _isWerewolf = (atoi(data) != 0) ? true : false;
            }
        }
        {
            const char *data = xmlElement->Attribute("cpu_units");
            if (data && strlen(data) > 0) {
                Utils::split(_cpuUnits, data, ",");
            }
        }
    }
}

MapUnitConfigData::~MapUnitConfigData()
{
    
}

int MapUnitConfigData::getMapId() const
{
    return _mapId;
}

bool MapUnitConfigData::isWerewolf() const
{
    return _isWerewolf;
}

const vector<string>& MapUnitConfigData::getCpuUnits() const
{
    return _cpuUnits;
}
