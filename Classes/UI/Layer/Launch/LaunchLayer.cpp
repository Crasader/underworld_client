//
//  LaunchLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 2016/9/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LaunchLayer.h"
#include "GameData.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "MainScene.h"
#include "MessageBoxLayer.h"
#include "XButton.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif

LaunchLayer* LaunchLayer::create()
{
    auto ret = new (std::nothrow) LaunchLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LaunchLayer::LaunchLayer()
:_login(nullptr) {}

LaunchLayer::~LaunchLayer()
{
    removeAllChildren();
}

bool LaunchLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto login(XButton::create(XButton::BSize::Big, XButton::BType::Blue));
        login->setPosition(Point(winSize.width / 2, winSize.height / 4));
        login->setTitleText("登录");
        login->addClickEventListener([this](Ref*) {
            SoundManager::getInstance()->playButtonSound();
            loginOrSignUp();
        });
        login->setVisible(false);
        addChild(login);
        _login = login;
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(LaunchLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(LaunchLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void LaunchLayer::onEnter()
{
    LayerColor::onEnter();
    autoLogin();
}

bool LaunchLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void LaunchLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - private
void LaunchLayer::loginOrSignUp()
{
    GameData::getInstance()->login(CC_CALLBACK_0(LaunchLayer::onLoginSuccessfully, this), [](int) {
        MessageBoxLayer::getInstance()->show(LocalHelper::getString("networkResponse_loadUserInfoFailed"));
    });
}

void LaunchLayer::autoLogin()
{
    GameData::getInstance()->autoLogin(CC_CALLBACK_0(LaunchLayer::onLoginSuccessfully, this), CC_CALLBACK_0(LaunchLayer::onAutoLoginFailed, this));
}

void LaunchLayer::onLoginSuccessfully()
{
    auto user(GameData::getInstance()->currentUser());
    auto director = Director::getInstance();
    if (user->isInBattle()) {
        // TODO:
    } else {
        director->replaceScene(MainScene::create());
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        iOSApi::initIAPEnviroment();
#endif
    }
}

void LaunchLayer::onAutoLoginFailed()
{
    if (_login) {
        _login->setVisible(true);
    }
}
