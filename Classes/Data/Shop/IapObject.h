//
//  IapObject.h
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef IapObject_h
#define IapObject_h

#include "cocos2d.h"

class IapObject
{
public:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IapObject(const std::string& key, void *value);
#endif
    virtual ~IapObject();
    
    void setIndex(int index);
    int index() const;
    int objectId() const;
    int count() const;
    float price() const;
    const std::string& title() const;
    const std::string& description() const;
    const std::string& priceString() const;
    
private:
    int _index;
    int _objectId;
    int _count;
    float _price;
    std::string _title;
    std::string _description;
    std::string _priceString;
};

#endif /* IapObject_h */
