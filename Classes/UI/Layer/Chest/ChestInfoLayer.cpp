//
//  ChestInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ChestInfoLayer.h"
#include "CocosGlobal.h"
#include "SmallBoard.h"

using namespace std;

ChestInfoLayer* ChestInfoLayer::create()
{
    auto ret = new (nothrow) ChestInfoLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChestInfoLayer::ChestInfoLayer()
:_observer(nullptr) {}

ChestInfoLayer::~ChestInfoLayer()
{
    removeAllChildren();
}

void ChestInfoLayer::registerObserver(ChestInfoLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool ChestInfoLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = SmallBoard::create();
        board->setTitle("untitled");
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(ChestInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(ChestInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool ChestInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void ChestInfoLayer::onTouchEnded(Touch *touch, Event *unused_event) {}
