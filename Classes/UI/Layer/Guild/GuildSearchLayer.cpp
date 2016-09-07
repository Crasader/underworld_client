//
//  GuildSearchLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/6.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildSearchLayer.h"
#include "CocosGlobal.h"
#include "Board.h"

using namespace std;

GuildSearchLayer* GuildSearchLayer::create()
{
    auto ret = new (nothrow) GuildSearchLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildSearchLayer::GuildSearchLayer() {}

GuildSearchLayer::~GuildSearchLayer()
{
    removeAllChildren();
}

bool GuildSearchLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void GuildSearchLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

bool GuildSearchLayer::init()
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
        eventListener->onTouchBegan = CC_CALLBACK_2(GuildSearchLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(GuildSearchLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}
