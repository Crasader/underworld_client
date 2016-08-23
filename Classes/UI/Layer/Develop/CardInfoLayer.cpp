//
//  CardInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardInfoLayer.h"
#include "DeckCard.h"
#include "UniversalButton.h"
#include "CocosGlobal.h"

using namespace std;

#pragma mark - PropertyNode
class CardInfoLayer::PropertyNode : public Node {
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

CardInfoLayer::PropertyNode* CardInfoLayer::PropertyNode::create()
{
    auto ret = new (nothrow) CardInfoLayer::PropertyNode();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoLayer::PropertyNode::PropertyNode()
:_icon(nullptr)
,_name(nullptr)
,_data(nullptr) {}

CardInfoLayer::PropertyNode::~PropertyNode()
{
    removeAllChildren();
}

bool CardInfoLayer::PropertyNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        return true;
    }
    
    return false;
}

void CardInfoLayer::PropertyNode::setProperty()
{
    
}

#pragma mark - CardInfoLayer
CardInfoLayer* CardInfoLayer::create()
{
    auto ret = new (nothrow) CardInfoLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoLayer::CardInfoLayer()
:_observer(nullptr)
,_background(nullptr)
,_icon(nullptr)
,_level(nullptr)
,_profession(nullptr)
,_description(nullptr) {}

CardInfoLayer::~CardInfoLayer()
{
    removeAllChildren();
}

bool CardInfoLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
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

void CardInfoLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

void CardInfoLayer::registerObserver(CardInfoLayerObserver *observer)
{
    _observer = observer;
}

void CardInfoLayer::setCard(int cardId)
{
    _icon->update(cardId);
}

int CardInfoLayer::getCard() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}
