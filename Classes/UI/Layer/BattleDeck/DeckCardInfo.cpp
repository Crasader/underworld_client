//
//  DeckCardInfo.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckCardInfo.h"
#include "DeckCard.h"
#include "UniversalButton.h"
#include "CocosGlobal.h"

using namespace std;

#pragma mark - PropertyNode
class DeckCardInfo::PropertyNode : public Node {
public:
    static PropertyNode* create();
    virtual ~PropertyNode();
    void setProperty();
    
private:
    PropertyNode();
    virtual bool init() override;
    
private:
    Sprite* _icon;
    Label* _name;
    Label* _data;
};

DeckCardInfo::PropertyNode* DeckCardInfo::PropertyNode::create()
{
    auto ret = new (nothrow) DeckCardInfo::PropertyNode();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DeckCardInfo::PropertyNode::PropertyNode()
:_icon(nullptr)
,_name(nullptr)
,_data(nullptr) {}

DeckCardInfo::PropertyNode::~PropertyNode()
{
    removeAllChildren();
}

bool DeckCardInfo::PropertyNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        return true;
    }
    
    return false;
}

void DeckCardInfo::PropertyNode::setProperty()
{
    
}

#pragma mark - DeckCardInfo
DeckCardInfo* DeckCardInfo::create()
{
    auto ret = new (nothrow) DeckCardInfo();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DeckCardInfo::DeckCardInfo()
:_observer(nullptr)
,_background(nullptr)
,_icon(nullptr)
,_level(nullptr)
,_profession(nullptr)
,_description(nullptr) {}

DeckCardInfo::~DeckCardInfo()
{
    removeAllChildren();
}

bool DeckCardInfo::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(DeckCardInfo::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(DeckCardInfo::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool DeckCardInfo::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    const auto& point(pTouch->getLocation());
    Rect rect(getBoundingBox());
    rect.origin = getParent()->convertToWorldSpace(rect.origin);
    if (rect.containsPoint(point)) {
        return true;
    }
    
    return false;
}

void DeckCardInfo::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

void DeckCardInfo::registerObserver(DeckCardInfoObserver *observer)
{
    _observer = observer;
}

void DeckCardInfo::setCard(int cardId)
{
    _icon->update(cardId);
}

int DeckCardInfo::getCard() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}
