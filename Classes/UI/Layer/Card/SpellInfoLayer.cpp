//
//  SpellInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SpellInfoLayer.h"
#include "DevelopCard.h"
#include "CardPropertyNode.h"
#include "DeckManager.h"
#include "MediumBoard.h"
#include "UniversalButton.h"
#include "CocosGlobal.h"
#include "LocalHelper.h"
#include "CardSimpleData.h"

using namespace std;

#pragma mark - SpellInfoLayer
SpellInfoLayer* SpellInfoLayer::create(int cardId)
{
    auto ret = new (nothrow) SpellInfoLayer();
    if (ret && ret->init(cardId)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SpellInfoLayer::SpellInfoLayer()
:_observer(nullptr)
,_icon(nullptr)
,_description(nullptr)
,_data(nullptr) {}

SpellInfoLayer::~SpellInfoLayer()
{
    removeAllChildren();
}

bool SpellInfoLayer::init(int cardId)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        _data = DeckManager::getInstance()->getCardData(cardId);
        
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = MediumBoard::create();
        board->setTitle(_data->getName());
        board->setExitCallback([this]() {
            if (_observer) {
                _observer->onSpellInfoLayerExit(this);
            }
        });
        board->setButtonTitle(LocalHelper::getString("Upgrade"));
        board->setButtonCallback([this]() {
            
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(SpellInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(SpellInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool SpellInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void SpellInfoLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

void SpellInfoLayer::registerObserver(SpellInfoLayerObserver *observer)
{
    _observer = observer;
}

int SpellInfoLayer::getCard() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}
