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
#include <map>

class AttributeLocalData;

class AttributeData
{
public:
    static void calculateAttributes(const std::map<int, AttributeData *>& attributes, std::map<int, int>& output);
    
    AttributeData(const std::string& content);
    AttributeData(int id, int count);
    AttributeData(const AttributeData& instance);
    virtual ~AttributeData();
    
    int getId() const;
    const AttributeLocalData* getLocalData() const;
    int getCount() const;
    
private:
    int _id;
    int _count;
};

#endif /* AttributeData_h */
