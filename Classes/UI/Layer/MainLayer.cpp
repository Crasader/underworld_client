//
//  MainLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MainLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "SoundManager.h"
#include "MainUILayer.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

Scene* MainLayer::createScene()
{
    Scene* scene = Scene::create();
    MainLayer* layer = MainLayer::create();
    scene->addChild(layer);
    return scene;
}

MainLayer* MainLayer::create()
{
    MainLayer *ret = new (nothrow) MainLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MainLayer::MainLayer()
:_observer(nullptr)
,_uiLayer(nullptr)
,_mainNode(nullptr)
,_scrollView(nullptr)
{
    
}

MainLayer::~MainLayer()
{
    removeAllChildren();
}

void MainLayer::registerObserver(MainLayerObserver *observer)
{
    _observer = observer;
}

bool MainLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
#if true
        _mainNode = Sprite::create("GameImages/world_bg/world_1.png");
        _mainNode->setAnchorPoint(Point::ZERO);
#else
        static const string CsbFile("zhuchangjing.csb");
        _mainNode = CSLoader::createNode(CsbFile);
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        _mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
#endif
        
        const Size& nodeSize = _mainNode->getContentSize();
        _touchEnabled = (nodeSize.height > winSize.height) ? true : false;
        _scrollView = ui::ScrollView::create();
        _scrollView->setDirection(cocos2d::ui::ScrollView::Direction::BOTH);
        _scrollView->setTouchEnabled(_touchEnabled);
        _scrollView->setBounceEnabled(false);
        _scrollView->setContentSize(winSize);
        _scrollView->setPosition(Point::ZERO);
        _scrollView->setInnerContainerSize(_touchEnabled ? nodeSize : winSize);
        _scrollView->setSwallowTouches(!_touchEnabled);
        _scrollView->setScrollBarEnabled(false);
        _scrollView->addChild(_mainNode);
        addChild(_scrollView);
        
        // UI layer
        _uiLayer = MainUILayer::create();
        addChild(_uiLayer);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MainLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MainLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void MainLayer::onEnter()
{
    LayerColor::onEnter();
    
    if (_scrollView)
    {
        _scrollView->jumpToBottom();
    }
}

bool MainLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void MainLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
