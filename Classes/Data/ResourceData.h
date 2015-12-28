//
//  ResourceData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ResourceData_h
#define ResourceData_h

#include "CocosGlobal.h"

class ResourceData
{
public:
    ResourceData(const std::string& content);
    virtual ~ResourceData();
    
    ResourceType getId() const;
    int getCount() const;
    
private:
    ResourceType _id;
    int _count;
};

#endif /* ResourceData_h */
