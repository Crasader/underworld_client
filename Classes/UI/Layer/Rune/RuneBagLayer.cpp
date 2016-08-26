//
//  RuneBagLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneBagLayer.h"
#include "MediumBoard.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"

using namespace std;

#pragma mark - RuneBagLayer
RuneBagLayer* RuneBagLayer::create()
{
    auto ret = new (nothrow) RuneBagLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

RuneBagLayer::RuneBagLayer()
:_observer(nullptr) {}

RuneBagLayer::~RuneBagLayer()
{
    removeAllChildren();
}

bool RuneBagLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = MediumBoard::create();
        board->setTitle("Rune bag");
        board->setButtonTitle(LocalHelper::getString("OK"));
        board->setButtonCallback([this]() {
            
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(RuneBagLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(RuneBagLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool RuneBagLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void RuneBagLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

void RuneBagLayer::registerObserver(RuneBagLayerObserver *observer)
{
    _observer = observer;
}
