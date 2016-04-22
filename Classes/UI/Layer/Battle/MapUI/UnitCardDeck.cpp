//
//  UnitCardDeck.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UnitCardDeck.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "CCShake.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int topZOrder(1);
static const float costOffsetX(1.0f);

UnitCardDeck* UnitCardDeck::create(int count)
{
    UnitCardDeck *ret = new (nothrow) UnitCardDeck();
    if (ret && ret->init(count))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitCardDeck::UnitCardDeck()
:_nextCardNode(nullptr)
,_nextCardProgress(nullptr)
,_nextLabel(nullptr)
,_countLabel(nullptr)
,_costHint(nullptr)
,_nextCard(nullptr)
,_isShaking(false)
,_costStartPosition(Point::ZERO)
{
    
}

UnitCardDeck::~UnitCardDeck()
{
    removeAllChildren();
}

bool UnitCardDeck::init(int count)
{
    if (Node::init())
    {
        static const float x1(10.0f);
        static const float x2(20.0f);
        static const float x3(8.0f);
        
        static const float y1(5.0f);
        static const float y2(50.0f);
        
        const Size& nodeSize = CardNode::create(false)->getContentSize();
        const Size size(x1 * 2 + x2 + (count + 1) * nodeSize.width + (count - 1) * x3, y1 + y2 + nodeSize.height);
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
            const float x = x1 + x2 + nodeSize.width * (i + 1.5f) + x3 * i;
            _positions.push_back(Point(x, y));
        }
        
        // candidate
        {
            _insertActionStartPosition = Point(x1 + nodeSize.width / 2, y);
            
            
        }
        
        _nextLabel = CocosUtils::createLabel("Next:0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _nextLabel->setPosition(_insertActionStartPosition.x, y2 / 2);
        _background->addChild(_nextLabel, topZOrder);
        
        // effect below "max 10"
        static const string file("UI-beijingguang.csb");
        Node* effect = CSLoader::createNode(file);
        addChild(effect);
        timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, true);
        
        _countLabel = CocosUtils::createLabel("0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _background->addChild(_countLabel, topZOrder);
        
        Label* maxCountLabel = CocosUtils::createLabel(StringUtils::format("Max:%d", BATTLE_RESOURCE_MAX_COUNT), BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _background->addChild(maxCountLabel, topZOrder);
        
        Size progressSize(Size::ZERO);
        for (int i = 0; i < BATTLE_RESOURCE_MAX_COUNT; ++i) {
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
        
        const float midX = x1 + x2 + nodeSize.width + (count * (nodeSize.width + x3) - x3) / 2;
        _costStartPosition.x = midX - ((progressSize.width + costOffsetX) * BATTLE_RESOURCE_MAX_COUNT - costOffsetX)  / 2;
        _costStartPosition.y = y2 / 2;
        for (int i = 0; i < _resources.size(); ++i) {
            auto pt = _resources.at(i).second;
            if (pt) {
                const float offsetX = progressSize.width / 2 + (progressSize.width + costOffsetX) * i;
                pt->setPosition(_costStartPosition + Point(offsetX, 0));
            }
        }
        
        _countLabel->setPosition(_costStartPosition);
        maxCountLabel->setPosition(midX, (y2 - progressSize.height) / 2);
        effect->setPosition(_countLabel->getPosition());
        
        return true;
    }
    
    return false;
}

void UnitCardDeck::updateNextCard(const Card* card)
{
    bool same(_nextCard == card);
    if (!same && card && _nextCard) {
//        const CardType* nextCt = _nextCard->getCardType();
//        const CardType* ct = card->getCardType();
//        if (nextCt && ct && nextCt->getName() == ct->getName()) {
//            same = true;
//        }
    }
    
    if (!same) {
        if (card) {
            createNextCardNode(card);
        } else {
            createBlankNextCardNode();
        }
        
        _nextCard = card;
    } else if (!_nextCard) {
        createBlankNextCardNode();
    }
}

void UnitCardDeck::updateTimer(float time, float duration)
{
    if (_nextLabel) {
        _nextLabel->setString(StringUtils::format("Next:%d", static_cast<int>(ceil(time))));
    }
    
    if (_nextCardProgress) {
        _nextCardProgress->setPercentage(time / duration * 100.0f);
    }
    
    if (time >= duration) {
        shake();
    } else {
        stopShake();
    }
}

void UnitCardDeck::updateResource(const unordered_map<string, float>& resources)
{
    float count(0);
    static const string& resourceName(RESOURCE_NAME);
    if (resources.find(resourceName) != resources.end()) {
        count = resources.at(resourceName);
    }
    
    const size_t cnt(_resources.size());
    count = MIN(MAX(0, count), cnt);
    
    for (int i = 0; i < cnt; ++i) {
        Node* s = _resources.at(i).first;
        ProgressTimer* pt = _resources.at(i).second;
        bool show(false);
        if (pt) {
            if (i <= count - 1) {
                show = (s == nullptr);
                pt->setPercentage(100.0f);
            } else if (i < count) {
                const float percentage(count - i);
                show = (percentage >= 1);
                pt->setPercentage(100.0f * percentage);
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
            Node* effect = CSLoader::createNode(file);
            effect->setPosition(pt->getPosition());
            pt->getParent()->addChild(effect);
            timeline::ActionTimeline *action = CSLoader::createTimeline(file);
            effect->runAction(action);
            action->gotoFrameAndPlay(0, false);
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

#pragma mark shake
static float shake_action_tag = 2016;
void UnitCardDeck::shake()
{
    if (!_isShaking) {
        stopShake();
        _isShaking = true;
        
        if (_nextCardNode) {
            static float shake_duration = 0.4f;
            static float shake_strength = 2.0f;
            
            auto action = RepeatForever::create(CCShake::actionWithDuration(shake_duration, shake_strength, _insertActionStartPosition));
            action->setTag(shake_action_tag);
            _nextCardNode->runAction(action);
        }
    }
}

void UnitCardDeck::stopShake()
{
    if (_isShaking) {
        _isShaking = false;
    }
    
    if (_nextCardNode && _nextCardNode->getActionByTag(shake_action_tag)) {
        _nextCardNode->stopActionByTag(shake_action_tag);
        _nextCardNode->setPosition(_insertActionStartPosition);
    }
}

void UnitCardDeck::addCostHint(int cost)
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
                auto size(Size(cost * (ptSize.width + costOffsetX), ptSize.height));
                _costHint->setContentSize(size);
            }
        }
    } else {
        removeCostHint();
    }
}

void UnitCardDeck::removeCostHint()
{
    if (_costHint) {
        _costHint->removeFromParent();
        _costHint = nullptr;
    }
}

void UnitCardDeck::createNextCardNode(const Card* card)
{
    if (_nextCardNode) {
        auto cardNode = dynamic_cast<CardNode*>(_nextCardNode);
        if (cardNode) {
            cardNode->update(card, BATTLE_RESOURCE_MAX_COUNT);
        } else {
            _nextCardNode->removeFromParent();
            _nextCardNode = nullptr;
        }
    }
    
    if (!_nextCardNode) {
        auto node = CardNode::create(true);
        node->update(card, BATTLE_RESOURCE_MAX_COUNT);
        node->setPosition(_insertActionStartPosition);
        _background->addChild(node);
        createNextCardProgress(node);
        _nextCardNode = node;
    }
}

void UnitCardDeck::createBlankNextCardNode()
{
    if (_nextCardNode && !dynamic_cast<Sprite*>(_nextCardNode)) {
        _nextCardNode->removeFromParent();
        _nextCardNode = nullptr;
    }
    
    if (!_nextCardNode) {
        auto node = Sprite::create("GameImages/test/ui_kapaibeiian.png");
        node->setPosition(_insertActionStartPosition);
        _background->addChild(node);
        createNextCardProgress(node);
        _nextCardNode = node;
    }
}

void UnitCardDeck::createNextCardProgress(Node* parent)
{
    if (parent) {
        static int tag(100);
        if (!parent->getChildByTag(tag)) {
            const Size& size = parent->getContentSize();
            auto s = Sprite::create("GameImages/test/ui_iconzhezhao_white.png");
            auto pt = ProgressTimer::create(s);
            pt->setType(ProgressTimer::Type::RADIAL);
            pt->setReverseDirection(true);
            pt->setMidpoint(Point::ANCHOR_MIDDLE);
            pt->setPercentage(100);
            pt->setPosition(Point(size.width / 2, size.height / 2));
            pt->setTag(tag);
            parent->addChild(pt);
            _nextCardProgress = pt;
        }
    }
}
