//
//  SkillUpgradeProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SkillUpgradeProperty_h
#define SkillUpgradeProperty_h

#include "AbstractUpgradeProperty.h"

class SkillUpgradeProperty : public AbstractUpgradeProperty
{
public:
    explicit SkillUpgradeProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~SkillUpgradeProperty();
    
    const std::pair<int, int>& getBookCost() const;
    
private:
    std::pair<int, int> _bookCost;
};

#endif /* SkillUpgradeProperty_h */
