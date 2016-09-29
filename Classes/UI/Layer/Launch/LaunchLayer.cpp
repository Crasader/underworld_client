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
#include "CocosUtils.h"
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
:_updater(nullptr)
,_progressTimer(nullptr)
,_progressHint(nullptr)
,_login(nullptr)
,_progressStep(0) {}

LaunchLayer::~LaunchLayer()
{
    removeAllChildren();
}

bool LaunchLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto sprite(Sprite::create("GameImages/test/zi_alpha version.png"));
        sprite->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(sprite);
        
        auto pt = ProgressTimer::create(Sprite::create("GameImages/test/zi_alpha version_progress.png"));
        pt->setType(ProgressTimer::Type::BAR);
        pt->setBarChangeRate(Vec2(1.0f, 0.0f));
        pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
        pt->setPercentage(0);
        pt->setPosition(winSize.width / 2, winSize.height / 4);
        addChild(pt);
        _progressTimer = pt;
        
        auto hint = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        hint->setPosition(pt->getPosition());
        addChild(hint);
        _progressHint = hint;
        
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
    showUpdater();
}

void LaunchLayer::onExit()
{
    removeUpdater();
    
    LayerColor::onExit();
}

bool LaunchLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void LaunchLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - OnlineUpdaterListener
void LaunchLayer::onLocalManifestNotExist()
{
    onUpdateFinished();
}

void LaunchLayer::onUpdateFinished()
{
    removeUpdater();
    
    GameData::getInstance()->parseFiles();
    
    showVersion();
    
#if ENABLE_LOGIN
    autoLogin();
#else
    onLoginSuccessfully();
#endif
}

void LaunchLayer::onUpdateFailed(EventAssetsManagerEx::EventCode error)
{
    MessageBoxLayer::getInstance()->show(LocalHelper::getString("hint_onlineUpdateFailed"), MessageBoxType::OnlyYes, [this](Ref *) {
        MessageBoxLayer::getInstance()->hide();
        removeUpdater();
        showUpdater();
    });
}

void LaunchLayer::onUpdateProgressed(float percentage)
{
    std::string percentFormat;
    if (_progressStep % 3 == 0) {
        percentFormat = ".  %.0f%%";
    } else if (_progressStep % 3 == 1) {
        percentFormat = ".. %.0f%%";
    } else {
        percentFormat = "...%.0f%%";
    }
    
    if (_progressTimer) {
        _progressTimer->setPercentage(percentage);
    }
    
    if (_progressHint) {
        _progressHint->setString(StringUtils::format(percentFormat.c_str(), percentage));
    }
    
     ++ _progressStep;
}

#pragma mark - private
void LaunchLayer::showVersion()
{
    const auto& winSize = Director::getInstance()->getWinSize();
    auto versionMessage = GameData::getInstance()->getVersionId();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    versionMessage += " Build：" + iOSApi::getBuildId();
#else
    
#endif
    auto versionLabel = CocosUtils::createLabel(LocalHelper::getString("hint_versionId") + versionMessage, DEFAULT_FONT_SIZE);
    versionLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    addChild(versionLabel);
    versionLabel->setPosition(Point(20.0f, winSize.height - 20.0f));
}

void LaunchLayer::showUpdater()
{
    _progressStep = 0;
    
    if (_progressTimer) {
        _progressTimer->setVisible(true);
        _progressTimer->setPercentage(0);
    }
    
    if (_progressHint) {
        _progressHint->setVisible(true);
        _progressHint->setString("0%");
    }
    
    if (!_updater) {
        _updater = new (std::nothrow) OnlineUpdater();
    }
    
    _updater->startUpdate(this);
}

void LaunchLayer::removeUpdater()
{
    if (_progressTimer) {
        _progressTimer->setVisible(false);
    }
    
    if (_progressHint) {
        _progressHint->setVisible(false);
    }
    
    if (_updater) {
        CC_SAFE_DELETE(_updater);
    }
}

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
