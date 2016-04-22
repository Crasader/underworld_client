//
//  CardDeck.cpp
//  Underworld_Client
//
//  Created by Andy on 16/4/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardDeck.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const float animationDuration(0.3f);
static const int cardActionTag(329);

CardDeck* CardDeck::create(int count)
{
    CardDeck *ret = new (nothrow) CardDeck();
    if (ret && ret->init(count)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardDeck::CardDeck()
:_observer(nullptr)
,_background(nullptr)
,_insertActionStartPosition(Point::ZERO)
{
    
}

CardDeck::~CardDeck()
{
    removeAllChildren();
}

bool CardDeck::init(int count)
{
    if (Node::init())
    {
        static const float x1(10.0f);
        static const float x3(8.0f);
        
        static const float y1(5.0f);
        static const float y2(50.0f);
        
        const Size& nodeSize = CardNode::create(false)->getContentSize();
        const Size size(x1 * 2 + count * nodeSize.width + (count - 1) * x3, y1 + y2 + nodeSize.height);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        // background
        Rect rect(0, 0, 91, 157);
        static const float capInsets(18.0f);
        Rect capInsetsRect(capInsets, capInsets, rect.size.width - capInsets * 2, rect.size.height - capInsets * 2);
        _background = Scale9Sprite::create("GameImages/test/ui_black_13.png", rect, capInsetsRect);
        _background->setContentSize(size);
        _background->setPosition(Point(size.width / 2, size.height / 2));
        addChild(_background);
        
        // children
        const float y = y2 + nodeSize.height / 2;
        for (int i = count - 1; i >= 0; --i) {
            const float x = x1 + nodeSize.width * (i + 0.5f) + x3 * i;
            _positions.push_back(Point(x, y));
        }
        
        return true;
    }
    
    return false;
}

void CardDeck::registerObserver(CardDeckObserver *observer)
{
    _observer = observer;
}

string CardDeck::getCard(int idx) const
{
    if (_nodes.size() > idx && idx >= 0) {
        CardNode* node = _nodes.at(idx);
        return node->getCardName();
    }
    
    return "";
}

void CardDeck::select(int idx)
{
    int cost(0);
    for (int i = 0; i < _nodes.size(); ++i) {
        CardNode* node = _nodes.at(i);
        if (node) {
            const bool selected(idx == i);
            node->setSelected(selected);
            
            // add hint
            if (selected) {
//                const Card* card = node->getCard();
//                if (card) {
//                    const CardType* ct = card->getCardType();
//                    if (ct) {
//                        auto costs = ct->getCost();
//                        static const string& name(RESOURCE_NAME);
//                        if (costs.find(name) != costs.end()) {
//                            cost = costs.at(name) / GameConstants::MICRORES_PER_RES;
//                        }
//                    }
//                }
            }
        }
    }
    
    addCostHint(cost);
}

void CardDeck::insert(const string& name, bool animated)
{
    auto node = insert(animated);
    node->update(name);
}

void CardDeck::insert(const Card* card, bool animated)
{
    auto node = insert(animated);
    node->update(card, BATTLE_RESOURCE_MAX_COUNT);
}

void CardDeck::remove(const Card* card, int index, bool animated)
{
    if (card) {
        function<void(int)> callback = [this](int index) {
            _nodes.at(index)->removeFromParent();
            _nodes.erase(_nodes.begin() + index);
            reload();
        };
        
        if (_nodes.size() > index) {
            auto node = _nodes.at(index);
            if (node->getCard() == card) {
                if (animated) {
                    node->runAction(Sequence::create(FadeOut::create(animationDuration), CallFunc::create([=] {
                        callback(index);
                    }), nullptr));
                } else {
                    callback(index);
                }
            }
        }
    }
}

void CardDeck::updateCD(int idx, float percentage)
{
    if (idx < _nodes.size()) {
        auto node = _nodes.at(idx);
        node->updateCD(percentage);
    }
}

#pragma mark - private
CardNode* CardDeck::insert(bool animated)
{
    const size_t idx(_nodes.size());
    const size_t cnt(_positions.size());
    CardNode* node = CardNode::create(true);
    node->registerObserver(this);
    if (idx < cnt) {
        const Point& point = _positions.at(idx);
        if (animated) {
            node->setPosition(_insertActionStartPosition);
            Action* action = MoveTo::create(animationDuration, point);
            action->setTag(cardActionTag);
            node->runAction(action);
        } else {
            node->setPosition(point);
        }
        node->setTag((int)idx);
        
        _nodes.push_back(node);
    } else {
        node->setVisible(false);
        _buffers.push(node);
    }
    addChild(node);
    
    return node;
}

void CardDeck::reload()
{
    const size_t maxCnt(_positions.size());
    const size_t cnt(_nodes.size());
    for (int i = 0; i < cnt; ++i) {
        auto node = _nodes.at(i);
        if (i < maxCnt) {
            const Point& position = _positions.at(i);
            if (node->getActionByTag(cardActionTag)) {
                node->stopActionByTag(cardActionTag);
            }
            node->setPosition(position);
            node->setTag(i);
        } else {
            node->removeFromParent();
        }
    }
    
    size_t i = cnt;
    while (_buffers.size() > 0 && i < maxCnt) {
        auto node = _buffers.front();
        node->setVisible(true);
        node->setPosition(_positions[i]);
        node->setTag((int)i);
        _nodes.push_back(node);
        _buffers.pop();
        ++ i;
    }
}

void CardDeck::addCostHint(int cost) {}

#pragma mark - CardNodeObserver
void CardDeck::onCardNodeTouchedBegan(CardNode* node)
{
    if (_observer) {
        _observer->onCardDeckTouchedBegan(this, node->getCardName(), node->getTag());
    }
}

void CardDeck::onCardNodeTouchedEnded(CardNode* node, bool isValid)
{
    if (isValid && _observer) {
        _observer->onCardDeckTouchedEnded(this, node->getCardName(), node->getTag());
    }
}
