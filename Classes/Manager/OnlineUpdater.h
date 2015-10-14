//
//  OnlineUpdater.h
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef OnlineUpdater_h
#define OnlineUpdater_h

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class OnlineUpdaterListener
{
public:
    virtual ~OnlineUpdaterListener() {}
    virtual void onLocalManifestNotExist() = 0;
    virtual void onUpdateFinished() = 0;
    virtual void onUpdateFailed(EventAssetsManagerEx::EventCode error) = 0;
    virtual void onUpdateProgressed(float percentage) = 0;
};

class OnlineUpdater
{
public:
    static OnlineUpdater * create();
    static void destroy(OnlineUpdater *instance);
    
    void startUpdate(OnlineUpdaterListener * listener);
    
private:
    OnlineUpdater();
    virtual ~OnlineUpdater();
    CC_DISALLOW_COPY_AND_ASSIGN(OnlineUpdater);
    
private:
    OnlineUpdaterListener * _listener;
    AssetsManagerEx * _am;
    EventListenerAssetsManagerEx * _amListener;
};

#endif /* OnlineUpdater_h */
