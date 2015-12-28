//
//  AttributeData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AttributeData_h
#define AttributeData_h

#include <iostream>

class AttributeLocalData;

class AttributeData
{
public:
    AttributeData(const std::string& content);
    virtual ~AttributeData();
    
    int getId() const;
    const AttributeLocalData* getLocalData() const;
    int getCount() const;
    
private:
    int _id;
    int _count;
};

#endif /* AttributeData_h */
