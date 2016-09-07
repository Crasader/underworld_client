//
//  ChestLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ChestLayer.h"
#include "BigBoard.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"

using namespace std;

ChestLayer* ChestLayer::create()
{
    auto ret = new (nothrow) ChestLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChestLayer::ChestLayer()
:_observer(nullptr)
,_scrollView(nullptr) {}

ChestLayer::~ChestLayer()
{
    removeAllChildren();
}

void ChestLayer::registerObserver(ChestLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool ChestLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = BigBoard::create();
        board->setTitle(LocalHelper::getString("ui_setting_title"));
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(ChestLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(ChestLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool ChestLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void ChestLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - ChestNodeObserver
