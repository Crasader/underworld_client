//
//  EditDeckMask.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "EditDeckMask.h"
#include "CocosGlobal.h"

using namespace std;

EditDeckMask* EditDeckMask::create(const Size& size)
{
    auto ret = new (nothrow) EditDeckMask();
    if (ret && ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

EditDeckMask::EditDeckMask()
:_observer(nullptr) {}

EditDeckMask::~EditDeckMask()
{
    removeAllChildren();
}

bool EditDeckMask::init(const Size& size)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR, size.width, size.height)) {
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(EditDeckMask::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(EditDeckMask::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool EditDeckMask::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    const auto& point(pTouch->getLocation());
    Rect rect(getBoundingBox());
    rect.origin = getParent()->convertToWorldSpace(rect.origin);
    if (rect.containsPoint(point)) {
        return true;
    }
    
    return false;
}

void EditDeckMask::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_observer) {
        _observer->onEditDeckMaskTouched(touch->getLocation());
    }
}

void EditDeckMask::registerObserver(EditDeckMaskObserver *observer)
{
    _observer = observer;
}
