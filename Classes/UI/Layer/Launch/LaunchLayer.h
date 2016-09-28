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

USING_NS_CC;

class XButton;

class LaunchLayer : public LayerColor
{
public:
    static LaunchLayer* create();
    virtual ~LaunchLayer();
    
protected:
    LaunchLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual void onEnter() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void loginOrSignUp();
    void autoLogin();
    void onLoginSuccessfully();
    void onAutoLoginFailed();
    
private:
    XButton* _login;
};

#endif /* LaunchLayer_h */
