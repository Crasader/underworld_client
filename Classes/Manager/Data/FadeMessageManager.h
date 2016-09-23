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

class FadeMessageManager
{
public:
    static FadeMessageManager* getInstance();
    static void purge();
    
    void show(const std::string& message);
    
private:
    FadeMessageManager();
    virtual ~FadeMessageManager();
    FadeMessageManager(const FadeMessageManager &) = delete;
    FadeMessageManager &operator =(const FadeMessageManager &) = delete;
    
private:
    std::queue<Label*> _labels;
};

#endif /* FadeMessageManager_h */
