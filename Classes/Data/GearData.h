//
//  GearData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearData_h
#define GearData_h

#include <iostream>
#include "CocosGlobal.h"

namespace tinyxml2 { class XMLElement; }

// =====================================================
// Gear Data
// =====================================================

class GearData
{
public:
    GearData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearData();
    
    GearType getType() const;
    GearQuality getQuality() const;
    const std::string& getDescription() const;
    
private:
    GearType _type;
    GearQuality _quality;
    std::string _description;
};

#endif /* GearData_h */
