//
//  ProgressLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ProgressLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"

static const std::string resourceFile("loading5.csb");
static const int ProgressLayerTag = 20150;

void ProgressLayer::showSingleton()
{
    auto runningScene = Director::getInstance()->getRunningScene();
    if (runningScene) {
        auto layer = ProgressLayer::create();
        runningScene->addChild(layer, 20000, ProgressLayerTag);
    }
}

void ProgressLayer::hideSingleton()
{
    auto runningScene = Director::getInstance()->getRunningScene();
    if (runningScene) {
        auto child = runningScene->getChildByTag(ProgressLayerTag);
        if (child) {
            runningScene->removeChild(child);
        }
    }
}

ProgressLayer * ProgressLayer::create()
{
    auto ret = new (std::nothrow) ProgressLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ProgressLayer::destroy(ProgressLayer* layer)
{
    CC_SAFE_DELETE(layer);
}

ProgressLayer::ProgressLayer() {}

ProgressLayer::~ProgressLayer()
{
    removeAllChildren();
}

bool ProgressLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR))
    {
        const auto& winSize = Director::getInstance()->getWinSize();
        auto node = CocosUtils::playAnimation(resourceFile, 0, true);
        node->setPosition(winSize.width / 2, winSize.height / 2);
        addChild(node);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(ProgressLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(ProgressLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        return true;
    }
    
    return false;
}

bool ProgressLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void ProgressLayer::onTouchEnded(Touch *touch, Event *unused_event) {}