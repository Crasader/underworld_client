//
//  SoldierQualityData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierQualityData_h
#define SoldierQualityData_h

#include "AbstractUpgradeData.h"

class AttributeData;

class SoldierQualityData : public AbstractUpgradeData
{
public:
    SoldierQualityData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierQualityData();
    
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    int getOutput() const;
    
private:
    std::map<int, AttributeData *> _attributes;
    int _output;
};

#endif /* SoldierQualityData_h */
