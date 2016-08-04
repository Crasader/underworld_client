//
//  CheatConfiguration.h
//  Underworld_Client
//
//  Created by Andy on 16/8/4.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CheatConfiguration_h
#define CheatConfiguration_h

#include "cocos2d.h"

class CheatConfiguration
{
public:
    static CheatConfiguration* getInstance();
    static void purge();
    
    bool loadPVR;
    
private:
    CheatConfiguration();
    virtual ~CheatConfiguration();
    CC_DISALLOW_COPY_AND_ASSIGN(CheatConfiguration);
};

#endif /* CheatConfiguration_h */
