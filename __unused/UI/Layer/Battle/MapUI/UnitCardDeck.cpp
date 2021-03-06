//
//  UnitCardDeck.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UnitCardDeck.h"
#include "DataManager.h"
#include "GameModeHMM.h"
#include "CocosUtils.h"
#include "CCShake.h"

using namespace std;

static const int topZOrder(1);

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
,_nextCard(nullptr)
,_isShaking(false)
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
        static const float x3(8.0f);
        
        static const float y1(50.0f);
        static const float y2(5.0f);
        
        const Size& nodeSize = CardNode::create(false)->getContentSize();
#if ENABLE_NEXT_CARD
        static const float x2(80.0f);
        const Size size(x1 * 2 + x2 + (count + 1) * nodeSize.width + (count - 1) * x3, y1 + y2 + nodeSize.height);
#else
        const Size size(x1 * 2 + count * nodeSize.width + (count - 1) * x3, y1 + y2 + nodeSize.height);
#endif
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
            const float x = x1 /*+ x2 */+ nodeSize.width * (i + 0.5f) + x3 * i;
            _positions.push_back(Point(x, y));
        }
        
        // candidate
        {
            _insertActionStartPosition = Point(size.width - (x1 + nodeSize.width / 2), y);
        }
        
#if false
        _nextLabel = CocosUtils::createLabel("Next:0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _nextLabel->setPosition(_insertActionStartPosition.x, y2 / 2);
        _background->addChild(_nextLabel, topZOrder);
#endif
        
        // effect below "max 10"
        static const string file("UI-beijingguang.csb");
        Node* effect = CocosUtils::playAnimation(file, 0, true);
        addChild(effect);
        
        _countLabel = CocosUtils::createLabel("0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _background->addChild(_countLabel, topZOrder);
        
        const auto resourceMaxCount(BATTLE_RESOURCE_MAX_COUNT);
#if false
        Label* maxCountLabel = CocosUtils::createLabel(StringUtils::format("Max:%d", resourceMaxCount), BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _background->addChild(maxCountLabel, topZOrder);
#endif
        
        Size progressSize(Size::ZERO);
        for (int i = 0; i < resourceMaxCount; ++i) {
            auto s = Sprite::create("GameImages/test/ui_blood_9.png");
            auto pt = ProgressTimer::create(s);
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
        
        const float midX = size.width / 2/*x1 + x2 + nodeSize.width + (count * (nodeSize.width + x3) - x3) / 2*/;
        _costStartPosition.x = midX - ((progressSize.width + deckCostOffsetX) * resourceMaxCount - deckCostOffsetX)  / 2;
        _costStartPosition.y = size.height - y1 / 2;
        for (int i = 0; i < _resources.size(); ++i) {
            auto pt = _resources.at(i).second;
            if (pt) {
                const float offsetX = progressSize.width / 2 + (progressSize.width + deckCostOffsetX) * i;
                pt->setPosition(_costStartPosition + Point(offsetX, 0));
            }
        }
        
        _countLabel->setPosition(_costStartPosition);
#if false
        maxCountLabel->setPosition(midX, (y2 - progressSize.height) / 2);
#endif
        effect->setPosition(_countLabel->getPosition());
        
        return true;
    }
    
    return false;
}

void UnitCardDeck::updateNextCard(const HMMCard* card)
{
    bool same(_nextCard == card);
    if (!same && card && _nextCard) {
        const HMMCardType* nextCt = _nextCard->getCardType();
        const HMMCardType* ct = card->getCardType();
        if (nextCt && ct && nextCt->getName() == ct->getName()) {
            same = true;
        }
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

void UnitCardDeck::createNextCardNode(const HMMCard* card)
{
    const auto resourceMaxCount(DataManager::getInstance()->getBattleResourceMaxCount());
    if (_nextCardNode) {
        auto cardNode = dynamic_cast<CardNode*>(_nextCardNode);
        if (cardNode) {
            cardNode->update(card, resourceMaxCount);
        } else {
            _nextCardNode->removeFromParent();
            _nextCardNode = nullptr;
        }
    }
    
    if (!_nextCardNode) {
        auto node = CardNode::create(true);
        node->update(card, resourceMaxCount);
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
