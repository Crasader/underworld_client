//
//  AbstractUpgradeProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AbstractUpgradeProperty_h
#define AbstractUpgradeProperty_h

#include "CocosGlobal.h"
#include <map>

namespace tinyxml2 { class XMLElement; }

class AbstractUpgradeProperty
{
public:
    explicit AbstractUpgradeProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~AbstractUpgradeProperty() = 0;
    
    int getId() const;
    int getLevel() const;
    std::pair<ResourceType, int> getResourceCost() const;
    
private:
    int _id;
    int _level;
    std::map<ResourceType, int> _cost;
};

#endif /* AbstractUpgradeProperty_h */
