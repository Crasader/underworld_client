//
//  GuildInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildInfoLayer.h"
#include "CocosGlobal.h"
#include "Board.h"

using namespace std;

GuildInfoLayer* GuildInfoLayer::create()
{
    auto ret = new (nothrow) GuildInfoLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildInfoLayer::GuildInfoLayer()
:_observer(nullptr) {}

GuildInfoLayer::~GuildInfoLayer()
{
    removeAllChildren();
}

void GuildInfoLayer::registerObserver(GuildInfoLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool GuildInfoLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(1);
        board->setTitle("untitled");
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(GuildInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(GuildInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool GuildInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void GuildInfoLayer::onTouchEnded(Touch *touch, Event *unused_event) {}
