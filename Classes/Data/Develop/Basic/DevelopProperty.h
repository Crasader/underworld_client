//
//  DevelopProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/9/22.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DevelopProperty_h
#define DevelopProperty_h

#include "AbstractProperty.h"
#include <vector>
#include <unordered_map>

class DevelopProperty : public AbstractProperty
{
public:
    explicit DevelopProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~DevelopProperty();
    
    const std::unordered_map<int, float>& getAttributes() const;
    float getAttribute(int type);
    const std::vector<int>& getOrderedAttributes() const;
    
private:
    int getAttributePriority(int aid) const;
    
private:
    std::unordered_map<int, float> _attributes;
    std::vector<int> _orderedAttributes;
};

#endif /* DevelopProperty_h */
