//
//  ContentData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ContentData_h
#define ContentData_h

#include <iostream>

class ContentData
{
public:
    ContentData(const std::string& content);
    virtual ~ContentData();
    
    int getType() const;
    int getId() const;
    int getCount() const;
    
private:
    int _type;
    int _id;
    int _count;
};

#endif /* ContentData_h */
