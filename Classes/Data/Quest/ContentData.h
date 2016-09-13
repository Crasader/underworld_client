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
#include <vector>

class ContentData
{
public:
    explicit ContentData(const std::string& content);
    virtual ~ContentData();
    
    int getType() const;
    int getId() const;
    int getCount(int index) const;
    
private:
    int _type;
    int _id;
    std::vector<int> _counts;
};

#endif /* ContentData_h */
