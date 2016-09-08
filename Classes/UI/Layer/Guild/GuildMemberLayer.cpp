//
//  GuildMemberLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildMemberLayer.h"
#include "CocosGlobal.h"
#include "BigBoard.h"
#include "UserGuildData.h"

using namespace std;

GuildMemberLayer* GuildMemberLayer::create(const UserGuildData* data)
{
    auto ret = new (nothrow) GuildMemberLayer();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildMemberLayer::GuildMemberLayer()
:_observer(nullptr)
,_data(nullptr) {}

GuildMemberLayer::~GuildMemberLayer()
{
    removeAllChildren();
}

void GuildMemberLayer::registerObserver(GuildMemberLayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - LayerColor
bool GuildMemberLayer::init(const UserGuildData* data)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = BigBoard::create();
        board->setTitle("Player Profile");
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(GuildMemberLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(GuildMemberLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool GuildMemberLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void GuildMemberLayer::onTouchEnded(Touch *touch, Event *unused_event) {}
