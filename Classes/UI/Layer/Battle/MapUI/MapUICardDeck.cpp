//
//  MapUICardDeck.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUICardDeck.h"
#include "Deck.h"
#include "CocosUtils.h"

using namespace std;
using namespace UnderWorld::Core;

static const int topZOrder(1);

MapUICardDeck* MapUICardDeck::create(int count)
{
    MapUICardDeck *ret = new (nothrow) MapUICardDeck();
    if (ret && ret->init(count))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MapUICardDeck::MapUICardDeck()
:_observer(nullptr)
,_candidateSprite(nullptr)
,_nextLabel(nullptr)
,_countLabel(nullptr)
{
    
}

MapUICardDeck::~MapUICardDeck()
{
    removeAllChildren();
}

bool MapUICardDeck::init(int count)
{
    if (Node::init())
    {
        static const float x1(10.0f);
        static const float x2(20.0f);
        static const float x3(8.0f);
        
        static const float y1(5.0f);
        static const float y2(50.0f);
        
        const Size& nodeSize = CardNode::create()->getContentSize();
        const Size size(x1 * 2 + x2 + (count + 1) * nodeSize.width + (count - 1) * x3, y1 + y2 + nodeSize.height);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        // background
        Rect rect(0, 0, 91, 157);
        static const float capInsets(18.0f);
        Rect capInsetsRect(capInsets, capInsets, rect.size.width - capInsets * 2, rect.size.height - capInsets * 2);
        Scale9Sprite* background = Scale9Sprite::create("GameImages/test/ui_black_13.png", rect, capInsetsRect);
        background->setContentSize(size);
        background->setPosition(Point(size.width / 2, size.height / 2));
        addChild(background);
        
        // children
        const float y = y2 + nodeSize.height / 2;
        for (int i = count - 1; i >= 0; --i) {
            const float x = x1 + x2 + nodeSize.width * (i + 1.5f) + x3 * i;
            _unitPositions.push_back(Point(x, y));
        }
        
        _candidateSprite = Sprite::create("GameImages/test/ui_kapaibeiian.png");
        _candidateSprite->setPosition(x1 + nodeSize.width / 2, y);
        background->addChild(_candidateSprite);
        
        _nextLabel = CocosUtils::createLabel("Next:0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _nextLabel->setPosition(_candidateSprite->getPosition().x, y2 / 2);
        background->addChild(_nextLabel, topZOrder);
        
        _countLabel = CocosUtils::createLabel("0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        background->addChild(_countLabel, topZOrder);
        
        Label* maxCountLabel = CocosUtils::createLabel(StringUtils::format("Max:%d", BATTLE_RESOURCE_MAX_COUNT), BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        background->addChild(maxCountLabel, topZOrder);
        
        Size progressSize(Size::ZERO);
        for (int i = 0; i < BATTLE_RESOURCE_MAX_COUNT; ++i) {
            Sprite* s = Sprite::create("GameImages/test/ui_blood_8.png");
            ProgressTimer* pt = ProgressTimer::create(s);
            pt->setType(ProgressTimer::Type::BAR);
            pt->setBarChangeRate(Vec2(1.0f, 0.0f));
            pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
            pt->setPercentage(100);
            background->addChild(pt);
            _resources.push_back(pt);
            
            if (progressSize.width == 0) {
                progressSize = pt->getContentSize();
            }
        }
        
        static const float offsetX(1.0f);
        const float midX = x1 + x2 + nodeSize.width + (count * (nodeSize.width + x3) - x3) / 2;
        const float startX = midX - ((progressSize.width + offsetX) * BATTLE_RESOURCE_MAX_COUNT - offsetX)  / 2;
        for (int i = 0; i < _resources.size(); ++i) {
            ProgressTimer* pt = _resources.at(i);
            const Point pos(startX + progressSize.width / 2 + (progressSize.width + offsetX) * i, y2 / 2);
            pt->setPosition(pos);
        }
        
        _countLabel->setPosition(startX, y2 / 2);
        maxCountLabel->setPosition(midX, (y2 - progressSize.height) / 2);
        
        return true;
    }
    
    return false;
}

void MapUICardDeck::registerObserver(MapUICardDeckObserver *observer)
{
    _observer = observer;
}

const Card* MapUICardDeck::getCard(int idx) const
{
    if (_cardNodes.size() > idx && idx >= 0) {
        CardNode* node = _cardNodes.at(idx);
        return node->getCard();
    }
    
    return nullptr;
}

void MapUICardDeck::select(int idx)
{
    for (int i = 0; i < _cardNodes.size(); ++i) {
        CardNode* node = _cardNodes.at(i);
        if (node) {
            node->setSelected(idx == i);
        }
    }
}

void MapUICardDeck::updateTimer(float time)
{
    if (_nextLabel) {
        _nextLabel->setString(StringUtils::format("Next:%d", static_cast<int>(time)));
    }
}

void MapUICardDeck::updateResource(const map<string, float>& resources)
{
    float count(0);
    static const string& resourceName(RES_NAME_WOOD);
    if (resources.find(resourceName) != resources.end()) {
        count = resources.at(resourceName);
    }
    
    const size_t cnt(_resources.size());
    count = MIN(MAX(0, count), cnt);
    
    for (int i = 0; i < cnt; ++i) {
        ProgressTimer* pt = _resources.at(i);
        if (pt) {
            if (i <= count - 1) {
                pt->setPercentage(100.0f);
            } else if (i < count) {
                pt->setPercentage(100.0f * (count - i));
            } else {
                pt->setPercentage(0);
            }
        }
    }
    
    if (_countLabel) {
        _countLabel->setString(StringUtils::format("%d", static_cast<int>(count)));
    }
    
    for (auto& node : _cardNodes) {
        node->checkResource(count);
    }
}

void MapUICardDeck::insert(const Card* card)
{
    createUnitNode(card);
}

void MapUICardDeck::remove(const Card* card)
{
    if (card) {
        bool update(false);
        for (auto iter = begin(_cardNodes); iter != end(_cardNodes); ++iter) {
            auto node = *iter;
            if (node->getCard() == card) {
                node->removeFromParent();
                _cardNodes.erase(iter);
                update = true;
                break;
            }
        }
        
        if (update) {
            reload();
        }
    }
}

#pragma mark - CardNodeObserver
void MapUICardDeck::onCardNodeTouchedBegan(CardNode* node)
{
    if (_observer) {
        _observer->onMapUICardDeckUnitTouchedBegan(node->getCard(), node->getTag());
    }
}

void MapUICardDeck::onCardNodeTouchedEnded(CardNode* node, bool isValid)
{
    if (isValid && _observer) {
        _observer->onMapUICardDeckUnitTouchedEnded(node->getCard(), node->getTag());
    }
}

void MapUICardDeck::createUnitNode(const Card* card)
{
    const size_t idx(_cardNodes.size());
    const size_t cnt(_unitPositions.size());
    CardNode* node = CardNode::create();
    node->update(card, BATTLE_RESOURCE_MAX_COUNT);
    node->registerObserver(this);
    node->setTag((int)idx);
    if (idx < cnt) {
        node->setPosition(_unitPositions.at(idx));
    } else {
        node->setVisible(false);
        _buffers.push(node);
    }
    addChild(node);
    _cardNodes.push_back(node);
}

void MapUICardDeck::reload()
{
    const size_t maxCnt(_unitPositions.size());
    const size_t cnt(_cardNodes.size());
    for (int i = 0; i < cnt; ++i) {
        auto node = _cardNodes.at(i);
        if (i < maxCnt) {
            const Point& position = _unitPositions.at(i);
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
        node->setPosition(_unitPositions[i++]);
        _cardNodes.push_back(node);
        _buffers.pop();
    }
}
