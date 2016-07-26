//
//  CardDeck.cpp
//  Underworld_Client
//
//  Created by Andy on 16/4/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardDeck.h"
#include "CocosUtils.h"
#include "DataManager.h"
#include "GameModeHMM.h"

using namespace std;
using namespace UnderWorld::Core;

static const float animationDuration(0.3f);
static const int cardActionTag(329);
const float deckCostOffsetX(1.0f);

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
,_countLabel(nullptr)
,_costHint(nullptr)
,_insertActionStartPosition(Point::ZERO)
,_costStartPosition(Point::ZERO)
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
        
        Size progressSize(Size::ZERO);
        const auto resourceMaxCount(BATTLE_RESOURCE_MAX_COUNT);
        for (int i = 0; i < resourceMaxCount; ++i) {
            Sprite* s = Sprite::create("GameImages/test/ui_blood_9.png");
            ProgressTimer* pt = ProgressTimer::create(s);
            pt->setType(ProgressTimer::Type::BAR);
            pt->setBarChangeRate(Vec2(1.0f, 0.0f));
            pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
            pt->setPercentage(100);
            _background->addChild(pt);
            
            _resources.push_back(make_pair(nullptr, pt));
            
            if (progressSize.width == 0) {
                progressSize = pt->getContentSize();
            }
        }
        
        const float midX = size.width / 2;
        _costStartPosition.x = midX - ((progressSize.width + deckCostOffsetX) * resourceMaxCount - deckCostOffsetX)  / 2;
        _costStartPosition.y = y2 / 2;
        for (int i = 0; i < _resources.size(); ++i) {
            auto pt = _resources.at(i).second;
            if (pt) {
                const float offsetX = progressSize.width / 2 + (progressSize.width + deckCostOffsetX) * i;
                pt->setPosition(_costStartPosition + Point(offsetX, 0));
            }
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
                cost = node->getCost();
            }
        }
    }
    
    addCostHint(cost);
}

void CardDeck::insert(const string& name, bool animated)
{
    auto node = insert(animated);
    node->updateWithoutInfo(name);
}

void CardDeck::insert(const HMMCard* card, bool animated)
{
    auto node = insert(animated);
    node->update(card, DataManager::getInstance()->getBattleResourceMaxCount());
}

void CardDeck::remove(const HMMCard* card, int index, bool animated)
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

void CardDeck::clear()
{
    for (int i = 0; i < _nodes.size(); ++i) {
        _nodes.at(i)->removeFromParent();
    }
    
    _nodes.clear();
}

void CardDeck::updateCD(int idx, float percentage)
{
    if (idx < _nodes.size()) {
        auto node = _nodes.at(idx);
        node->updateCD(percentage);
    }
}

void CardDeck::updateResource(const unordered_map<string, float>& resources)
{
    float count(0);
    static const string& resourceName(RES_NAME_WOOD);
    if (resources.find(resourceName) != resources.end()) {
        count = resources.at(resourceName);
    }
    
    const size_t cnt(_resources.size());
    auto resourceMaxCount(DataManager::getInstance()->getBattleResourceMaxCount());
    count = MIN(MAX(0, count), resourceMaxCount);
    
    float percentage(0);
    if (resourceMaxCount > 0) {
        percentage = count / resourceMaxCount * BATTLE_RESOURCE_MAX_COUNT;
    }
    
    for (int i = 0; i < cnt; ++i) {
        auto s = _resources.at(i).first;
        auto pt = _resources.at(i).second;
        bool show(false);
        if (pt) {
            if (i <= percentage - 1) {
                show = (s == nullptr);
                pt->setPercentage(100.0f);
            } else if (i < percentage) {
                const float p(percentage - i);
                show = (p >= 1);
                pt->setPercentage(100.0f * p);
                
                // remove if needed
                if (percentage < 1 && s) {
                    s->removeFromParent();
                    _resources.at(i).first = nullptr;
                }
            } else {
                if (s) {
                    s->removeFromParent();
                    _resources.at(i).first = nullptr;
                }
                pt->setPercentage(0);
            }
        }
        
        if (show) {
            static const string file("ui-tiao.csb");
            auto effect = CocosUtils::playAnimation(file, 0, false);
            effect->setPosition(pt->getPosition());
            pt->getParent()->addChild(effect);
            _resources.at(i).first = effect;
        }
    }
    
    if (_countLabel) {
        _countLabel->setString(StringUtils::format("%d", static_cast<int>(count)));
    }
    
    for (auto& node : _nodes) {
        node->checkResource(count);
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

void CardDeck::addCostHint(int cost)
{
    if (cost > 0) {
        if (!_costHint) {
            static string file("GameImages/test/ui_blood_10.png");
            static const Rect rect(0, 0, 63, 17);
            static const Rect capInsets(2, 2, 59, 13);
            _costHint = Scale9Sprite::create(file, rect, capInsets);
            _costHint->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            _costHint->setPosition(_costStartPosition);
            _background->addChild(_costHint);
        }
        
        if (_resources.size() > 0) {
            auto pt = _resources.at(0).second;
            if (pt) {
                auto ptSize(pt->getContentSize());
                auto resourceMaxCount(DataManager::getInstance()->getBattleResourceMaxCount());
                auto size(Size(cost * (ptSize.width + deckCostOffsetX) * BATTLE_RESOURCE_MAX_COUNT / resourceMaxCount, ptSize.height));
                _costHint->setContentSize(size);
            }
        }
    } else {
        removeCostHint();
    }
}

void CardDeck::removeCostHint()
{
    if (_costHint) {
        _costHint->removeFromParent();
        _costHint = nullptr;
    }
}

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
