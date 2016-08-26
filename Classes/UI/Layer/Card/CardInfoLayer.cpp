//
//  CardInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardInfoLayer.h"
#include "DevelopCard.h"
#include "CardPropertyNode.h"
#include "DeckManager.h"
#include "Board.h"
#include "UniversalButton.h"
#include "CocosGlobal.h"
#include "CardSimpleData.h"

using namespace std;

#pragma mark - CardInfoLayer
CardInfoLayer* CardInfoLayer::create(int cardId)
{
    auto ret = new (nothrow) CardInfoLayer();
    if (ret && ret->init(cardId)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoLayer::CardInfoLayer()
:_observer(nullptr)
,_icon(nullptr)
,_level(nullptr)
,_profession(nullptr)
,_description(nullptr)
,_data(nullptr) {}

CardInfoLayer::~CardInfoLayer()
{
    removeAllChildren();
}

bool CardInfoLayer::init(int cardId)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        _data = DeckManager::getInstance()->getCardData(cardId);
        
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(2);
        board->setTitle(_data->getName());
        board->setExitCallback([this]() {
            if (_observer) {
                _observer->onCardInfoLayerExit(this);
            }
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        UniversalButton::createReturnButton(board, Vec2(8.0f, 10.0f), [this]() {
            if (_observer) {
                _observer->onCardInfoLayerReturn(this);
            }
        });
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(CardInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(CardInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool CardInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    const auto& point(pTouch->getLocation());
    Rect rect(getBoundingBox());
    rect.origin = getParent()->convertToWorldSpace(rect.origin);
    if (rect.containsPoint(point)) {
        return true;
    }
    
    return false;
}

void CardInfoLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

void CardInfoLayer::registerObserver(CardInfoLayerObserver *observer)
{
    _observer = observer;
}

int CardInfoLayer::getCard() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}
