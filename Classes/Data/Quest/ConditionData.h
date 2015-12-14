//
//  ConditionData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ConditionData_h
#define ConditionData_h

#include <iostream>

class ConditionData
{
public:
    ConditionData(const std::string& content);
    virtual ~ConditionData();
    
    int getId() const;
    int getCount() const;
    
private:
    int _id;
    int _count;
};

#endif /* ConditionData_h */
