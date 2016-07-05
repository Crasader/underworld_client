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
    Scene *runningScene = cocos2d::Director::getInstance()->getRunningScene();
    if (runningScene)
    {
        ProgressLayer* layer = ProgressLayer::create();
        runningScene->addChild(layer, 20000, ProgressLayerTag);
    }
}

void ProgressLayer::hideSingleton()
{
    Scene *runningScene = cocos2d::Director::getInstance()->getRunningScene();
    if (runningScene)
    {
        Node* child = runningScene->getChildByTag(ProgressLayerTag);
        if (child) {
            runningScene->removeChild(child);
        }
    }
}

ProgressLayer * ProgressLayer::create()
{
    ProgressLayer *ret = new (std::nothrow) ProgressLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

void ProgressLayer::destroy(ProgressLayer* layer)
{
    CC_SAFE_DELETE(layer);
}

ProgressLayer::ProgressLayer()
{
}

ProgressLayer::~ProgressLayer()
{
    removeAllChildren();
}

bool ProgressLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        Node* loadingNode = CocosUtils::playAnimation(resourceFile, 0, true);
        loadingNode->setPosition(winSize.width / 2, winSize.height / 2);
        addChild(loadingNode);
        
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

void ProgressLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}