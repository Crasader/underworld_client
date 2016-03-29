//
//  HeroUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroUpgradeData_h
#define HeroUpgradeData_h

#include "AbstractUpgradeData.h"

class AttributeData;

class HeroUpgradeData : public AbstractUpgradeData
{
public:
    HeroUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~HeroUpgradeData();
    
    const std::unordered_map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    
private:
    std::unordered_map<int, AttributeData *> _attributes;
};

#endif /* HeroUpgradeData_h */
