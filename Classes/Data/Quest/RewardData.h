//
//  RewardData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef RewardData_h
#define RewardData_h

#include <iostream>

class RewardData
{
public:
    RewardData(const std::string& content);
    virtual ~RewardData();
    
    int getPercentage() const;
    int getId() const;
    int getCount() const;
    
private:
    int _percentage;
    int _id;
    int _count;
};

#endif /* RewardData_h */
