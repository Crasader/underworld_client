//
//  FadeMessageManager.h
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FadeMessageManager_h
#define FadeMessageManager_h

#include "cocos2d.h"

USING_NS_CC;

class ShopManager
{
public:
    static ShopManager* getInstance();
    static void purge();
    
    void show(const std::string& message);
    
private:
    ShopManager();
    virtual ~ShopManager();
    ShopManager(const ShopManager &) = delete;
    ShopManager &operator =(const ShopManager &) = delete;
    
private:
    std::queue<Label*> _labels;
};

#endif /* FadeMessageManager_h */
