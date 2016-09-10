//
//  MainLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MainLayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "GameManager.h"
#include "SoundManager.h"
#include "MainUILayer.h"
#include "LoadingLayer.h"

using namespace std;
using namespace ui;

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
,_touchInvalid(false) {}

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
        const auto& winSize = Director::getInstance()->getWinSize();
#if true
        _mainNode = Sprite::create("GameImages/world_bg/world_1.png");
        _mainNode->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
#else
        static const string CsbFile("zhuchangjing.csb");
        _mainNode = CocosUtils::playAnimation(CsbFile, 0, false);
#endif
        
        const auto& nodeSize(_mainNode->getContentSize());
        _touchEnabled = (nodeSize.width > winSize.width || nodeSize.height > winSize.height) ? true : false;
        _scrollView = extension::ScrollView::create();
        _scrollView->setDirection(extension::ScrollView::Direction::BOTH);
        _scrollView->setTouchEnabled(_touchEnabled);
        _scrollView->setBounceable(false);
        _scrollView->setViewSize(winSize);
        _scrollView->setContentSize(nodeSize);
        _scrollView->setPosition(Point::ZERO);
        _scrollView->addChild(_mainNode);
        addChild(_scrollView);
        
        const float scale(MAX(winSize.width / nodeSize.width, winSize.height / nodeSize.height));
        _scrollView->setMaxScale(1);
        _scrollView->setMinScale(scale);
        
        // UI layer
        _uiLayer = MainUILayer::create();
        addChild(_uiLayer);
        
        addLevelButtons();
        
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
    
    if (_scrollView) {
        _scrollView->setContentOffset(Vec2::ZERO);
    }
}

bool MainLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void MainLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

void MainLayer::addLevelButtons()
{
    static const size_t count(4);
    static const Vec2 positions[count] = {Vec2(0.5, 0.5), Vec2(0.47, 0.8), Vec2(0.75, 0.5), Vec2(0.8, 0.75)};
    const Size& size = _mainNode->getContentSize();
    for (int i = 0; i < count; ++i) {
        string file = StringUtils::format("GameImages/icons/guanqia/icon_guanka_%d.png", i + 1);
        auto button = Button::create(file, file);
        button->setPosition(Point(size.width * positions[i].x, size.height * positions[i].y));
        button->setSwallowTouches(false);
        _mainNode->addChild(button, 0, i);
        
        CocosUtils::fixWidgetTouchEvent(button, _touchInvalid, nullptr, [this](Ref* pSender) {
            // TODO:
            SoundManager::getInstance()->playButtonSound();
            
            auto widget = dynamic_cast<ui::Widget*>(pSender);
            auto mapId(widget->getTag() + 1);
            LoadingLayer::show(false, [mapId](Node*) {
                GameManager::getGameClient()->launchPve(mapId);
            });
        });
    }
}
