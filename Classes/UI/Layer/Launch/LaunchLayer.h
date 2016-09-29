//
//  LaunchLayer.h
//  Underworld_Client
//
//  Created by Andy on 2016/9/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef LaunchLayer_h
#define LaunchLayer_h

#include "cocos2d.h"
#include "OnlineUpdater.h"

USING_NS_CC;

class XButton;

class LaunchLayer
: public LayerColor
, public OnlineUpdaterListener
{
public:
    static LaunchLayer* create();
    virtual ~LaunchLayer();
    
protected:
    LaunchLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // OnlineUpdaterListener
    virtual void onLocalManifestNotExist() override;
    virtual void onUpdateFinished() override;
    virtual void onUpdateFailed(EventAssetsManagerEx::EventCode error) override;
    virtual void onUpdateProgressed(float percentage) override;
    
    void showVersion();
    void showUpdater();
    void removeUpdater();
    
    void loginOrSignUp();
    void autoLogin();
    void onLoginSuccessfully();
    void onAutoLoginFailed();
    
private:
    OnlineUpdater* _updater;
    ProgressTimer* _progressTimer;
    Label* _progressHint;
    XButton* _login;
    int _progressStep;
};

#endif /* LaunchLayer_h */
