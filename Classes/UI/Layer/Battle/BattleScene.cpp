//
//  BattleScene.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleScene.h"
#include "SoundManager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ApiBridge.h"
#endif

BattleScene* BattleScene::create()
{
    auto ret = new (std::nothrow) BattleScene();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleScene::BattleScene() {}

BattleScene::~BattleScene() {}

bool BattleScene::init()
{
    if (Scene::init()) {
        return true;
    }
    
    return false;
}

void BattleScene::onEnter()
{
    Scene::onEnter();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(true);
#endif
}

void BattleScene::onExit()
{
    SoundManager::getInstance()->stopBackgroundMusic();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    iOSApi::setMultipleTouchEnabled(false);
#endif
    Scene::onExit();
}
