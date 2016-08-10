//
//  MainScene.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "MainScene.h"
#include "MainLayer.h"
#include "SoundManager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif

MainScene* MainScene::create()
{
    auto ret = new (std::nothrow) MainScene();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MainScene::MainScene() {}

MainScene::~MainScene() {}

bool MainScene::init()
{
    if (Scene::init()) {
        auto layer = MainLayer::create();
        addChild(layer);
        return true;
    }
    
    return false;
}

void MainScene::onEnter()
{
    Scene::onEnter();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(true);
#endif
}

void MainScene::onExit()
{
    SoundManager::getInstance()->stopBackgroundMusic();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(false);
#endif
    Scene::onExit();
}
