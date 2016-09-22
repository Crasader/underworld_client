//
//  ObjectBriefData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ObjectBriefData_h
#define ObjectBriefData_h

#include <string>

class ObjectBriefData
{
public:
    explicit ObjectBriefData(const std::string& content);
    virtual ~ObjectBriefData();
    
    int getPercentage() const;
    int getId() const;
    int getCount() const;
    
private:
    int _percentage;
    int _id;
    int _count;
};

#endif /* ObjectBriefData_h */
