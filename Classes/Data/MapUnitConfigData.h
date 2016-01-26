//
//  MapUnitConfigData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/26.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUnitConfigData_h
#define MapUnitConfigData_h

#include <iostream>
#include <vector>

namespace tinyxml2 { class XMLElement; }

class MapUnitConfigData
{
public:
    MapUnitConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~MapUnitConfigData();
    
    int getMapId() const;
    bool isWerewolf() const;
    const std::vector<std::string>& getMyHeroes() const;
    const std::vector<std::string>& getMySoldiers() const;
    const std::vector<std::string>& getCpuUnits() const;
    
private:
    int _mapId;
    bool _isWerewolf;
    std::vector<std::string> _myHeroes;
    std::vector<std::string> _mySoldiers;
    std::vector<std::string> _cpuUnits;
};

#endif /* MapUnitConfigData_h */
