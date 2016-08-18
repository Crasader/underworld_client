//
//  DeckEditMask.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckEditMask.h"
#include "CocosGlobal.h"

using namespace std;

DeckEditMask* DeckEditMask::create(const Size& size)
{
    auto ret = new (nothrow) DeckEditMask();
    if (ret && ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DeckEditMask::DeckEditMask()
:_observer(nullptr) {}

DeckEditMask::~DeckEditMask()
{
    removeAllChildren();
}

bool DeckEditMask::init(const Size& size)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR, size.width, size.height)) {
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(DeckEditMask::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(DeckEditMask::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool DeckEditMask::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    const auto& point(pTouch->getLocation());
    Rect rect(getBoundingBox());
    rect.origin = getParent()->convertToWorldSpace(rect.origin);
    if (rect.containsPoint(point)) {
        return true;
    }
    
    return false;
}

void DeckEditMask::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_observer) {
        _observer->onDeckEditMaskTouched(touch->getLocation());
    }
}

void DeckEditMask::registerObserver(DeckEditMaskObserver *observer)
{
    _observer = observer;
}
