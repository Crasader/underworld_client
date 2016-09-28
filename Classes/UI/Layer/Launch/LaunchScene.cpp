//
//  LaunchScene.cpp
//  Underworld_Client
//
//  Created by Andy on 2016/9/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LaunchScene.h"
#include "LaunchLayer.h"

LaunchScene* LaunchScene::create()
{
    auto ret = new (std::nothrow) LaunchScene();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LaunchScene::LaunchScene() {}

LaunchScene::~LaunchScene() {}

bool LaunchScene::init()
{
    if (Scene::init()) {
        auto layer = LaunchLayer::create();
        addChild(layer);
        return true;
    }
    
    return false;
}
