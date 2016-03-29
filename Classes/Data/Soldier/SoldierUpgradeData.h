//
//  SoldierUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierUpgradeData_h
#define SoldierUpgradeData_h

#include "AbstractUpgradeData.h"

class AttributeData;

class SoldierUpgradeData : public AbstractUpgradeData
{
public:
    SoldierUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierUpgradeData();
    
    const std::unordered_map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
private:
    std::unordered_map<int, AttributeData *> _attributes;
};

#endif /* SoldierUpgradeData_h */
