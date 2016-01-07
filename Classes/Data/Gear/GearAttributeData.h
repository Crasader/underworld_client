//
//  GearAttributeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearAttributeData_h
#define GearAttributeData_h

#include "AbstractUpgradeData.h"

class GearAttributeData : public AbstractUpgradeData
{
public:
    GearAttributeData(tinyxml2::XMLElement *xmlElement);
    virtual ~GearAttributeData();
    
    int getAttributeId() const;
    int getAttributeValue(GearQuality type) const;
    
private:
    int _attributeId;
    std::map<GearQuality, int> _attributes;
};

#endif /* GearAttributeData_h */
