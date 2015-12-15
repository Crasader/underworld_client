//
//  GearAttribute.h
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef GearAttribute_h
#define GearAttribute_h

#include <iostream>

class GearAttribute
{
public:
    GearAttribute(const std::string& content);
    virtual ~GearAttribute();
    
    int getId() const;
    int getCount() const;
    
private:
    int _id;
    int _count;
};

#endif /* GearAttribute_h */
