//
//  GuildLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildLayer.h"
#include "CocosGlobal.h"
#include "Board.h"
#include "PureScale9Sprite.h"

using namespace std;

GuildLayer* GuildLayer::create()
{
    auto ret = new (nothrow) GuildLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildLayer::GuildLayer()
:_rightBackground(nullptr)
,_infoUINode(nullptr)
,_redEnvelopeUINode(nullptr)
,_donateUINode(nullptr) {}

GuildLayer::~GuildLayer()
{
    removeAllChildren();
}

#pragma mark - LayerColor
bool GuildLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(2);
        board->setTitle("untitled");
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto rightNode = board->getSubNode(1);
        const auto& rightSize(rightNode->getContentSize());
        static const float edge(5);
        auto background(PureScale9Sprite::create(PureScale9Sprite::Type::BlueDeep));
        background->setContentSize(Size(rightSize.width - edge * 2, rightSize.height - edge * 2));
        background->setPosition(rightSize.width / 2, rightSize.height / 2);
        rightNode->addChild(background);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(GuildLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(GuildLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool GuildLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void GuildLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - GuildUIInfoNodeObserverGuildUIInfoNodeObserver
void GuildLayer::onGuildUIInfoNodeDetail()
{
    
}

void GuildLayer::onGuildUIInfoNodeFriendship()
{
    
}

void GuildLayer::onGuildUIInfoNodeInteract()
{
    
}

void GuildLayer::onGuildUIInfoNodeDonate()
{
    
}

#pragma mark - GuildUIInteractNodeObserver

#pragma mark - GuildUIDonateNodeObserver
