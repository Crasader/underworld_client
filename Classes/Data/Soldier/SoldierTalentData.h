//
//  SoldierTalentData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoldierTalentData_h
#define SoldierTalentData_h

#include "AbstractUpgradeData.h"

class AttributeData;
class SkillLocalData;

class SoldierTalentData : public AbstractUpgradeData
{
public:
    SoldierTalentData(tinyxml2::XMLElement *xmlElement);
    virtual ~SoldierTalentData();
    
    const std::map<int, AttributeData *>& getAttributes() const;
    const AttributeData* getAttribute(int id) const;
    const SkillLocalData* getSkillData() const;
    
private:
    std::map<int, AttributeData *> _attributes;
    int _skillId;
};

#endif /* SoldierTalentData_h */
