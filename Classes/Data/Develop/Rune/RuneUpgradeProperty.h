//
//  RuneUpgradeProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneUpgradeProperty_h
#define RuneUpgradeProperty_h

#include "AbstractUpgradeProperty.h"

class RuneUpgradeProperty : public AbstractUpgradeProperty
{
public:
    RuneUpgradeProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~RuneUpgradeProperty();
    
    int getRuneCost() const;
    
private:
    int _runeCost;
};

#endif /* RuneUpgradeProperty_h */
