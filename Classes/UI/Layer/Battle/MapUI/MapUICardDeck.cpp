//
//  MapUICardDeck.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUICardDeck.h"
#include "cocostudio/CocoStudio.h"
#include "Deck.h"
#include "CocosUtils.h"
#include "CCShake.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int topZOrder(1);
static const float animationDuration(0.3f);
static const int cardActionTag(329);

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
,_candidateProgress(nullptr)
,_nextLabel(nullptr)
,_countLabel(nullptr)
,_isShaking(false)
,_candidateSpritePosition(Point::ZERO)
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
        
        const Size& nodeSize = CardNode::create(false)->getContentSize();
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
        
        // candidate
        {
            _candidateSprite = Sprite::create("GameImages/test/ui_kapaibeiian.png");
            _candidateSpritePosition = Point(x1 + nodeSize.width / 2, y);
            _candidateSprite->setPosition(_candidateSpritePosition);
            background->addChild(_candidateSprite);
            
            const Size& size = _candidateSprite->getContentSize();
            Sprite* s = Sprite::create("GameImages/test/ui_iconzhezhao_white.png");
            ProgressTimer* pt = ProgressTimer::create(s);
            pt->setType(ProgressTimer::Type::RADIAL);
            pt->setReverseDirection(true);
            pt->setMidpoint(Point::ANCHOR_MIDDLE);
            pt->setPercentage(100);
            pt->setPosition(Point(size.width / 2, size.height / 2));
            _candidateSprite->addChild(pt);
            
            _candidateProgress = pt;
        }
        
        _nextLabel = CocosUtils::createLabel("Next:0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _nextLabel->setPosition(_candidateSprite->getPosition().x, y2 / 2);
        background->addChild(_nextLabel, topZOrder);
        
        // effect below "max 10"
        static const string file("UI-beijingguang.csb");
        Node* effect = CSLoader::createNode(file);
        addChild(effect);
        timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, true);
        
        _countLabel = CocosUtils::createLabel("0", BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        background->addChild(_countLabel, topZOrder);
        
        Label* maxCountLabel = CocosUtils::createLabel(StringUtils::format("Max:%d", BATTLE_RESOURCE_MAX_COUNT), BIG_FONT_SIZE, DEFAULT_NUMBER_FONT);
        background->addChild(maxCountLabel, topZOrder);
        
        Size progressSize(Size::ZERO);
        for (int i = 0; i < BATTLE_RESOURCE_MAX_COUNT; ++i) {
            Sprite* s = Sprite::create("GameImages/test/ui_blood_9.png");
            ProgressTimer* pt = ProgressTimer::create(s);
            pt->setType(ProgressTimer::Type::BAR);
            pt->setBarChangeRate(Vec2(1.0f, 0.0f));
            pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
            pt->setPercentage(100);
            background->addChild(pt);
            
            _resources.push_back(make_pair(nullptr, pt));
            
            if (progressSize.width == 0) {
                progressSize = pt->getContentSize();
            }
        }
        
        static const float offsetX(1.0f);
        const float midX = x1 + x2 + nodeSize.width + (count * (nodeSize.width + x3) - x3) / 2;
        const float startX = midX - ((progressSize.width + offsetX) * BATTLE_RESOURCE_MAX_COUNT - offsetX)  / 2;
        for (int i = 0; i < _resources.size(); ++i) {
            const Point pos(startX + progressSize.width / 2 + (progressSize.width + offsetX) * i, y2 / 2);
            _resources.at(i).second->setPosition(pos);
        }
        
        _countLabel->setPosition(startX, y2 / 2);
        maxCountLabel->setPosition(midX, (y2 - progressSize.height) / 2);
        effect->setPosition(_countLabel->getPosition());
        
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

void MapUICardDeck::updateTimer(float time, float duration)
{
    if (_nextLabel) {
        _nextLabel->setString(StringUtils::format("Next:%d", static_cast<int>(ceil(time))));
    }
    
    if (_candidateProgress) {
        _candidateProgress->setPercentage(time / duration * 100.0f);
    }
    
    if (time >= duration) {
        shake();
    } else {
        stopShake();
    }
}

void MapUICardDeck::updateResource(const unordered_map<string, float>& resources)
{
    float count(0);
    static const string& resourceName(RES_NAME_WOOD);
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
    
    for (auto& node : _cardNodes) {
        node->checkResource(count);
    }
}

void MapUICardDeck::insert(const Card* card, bool animated)
{
    const size_t idx(_cardNodes.size());
    const size_t cnt(_unitPositions.size());
    CardNode* node = CardNode::create(true);
    node->update(card, BATTLE_RESOURCE_MAX_COUNT);
    node->registerObserver(this);
    node->setTag((int)idx);
    if (idx < cnt) {
        const Point& point = _unitPositions.at(idx);
        if (animated) {
            node->setPosition(_candidateSpritePosition);
            Action* action = MoveTo::create(animationDuration, point);
            action->setTag(cardActionTag);
            node->runAction(action);
        } else {
            node->setPosition(point);
        }
        
        _cardNodes.push_back(node);
    } else {
        node->setVisible(false);
        _buffers.push(node);
    }
    addChild(node);
}

void MapUICardDeck::remove(const Card* card, int index, bool animated)
{
    if (card) {
        function<void(int)> callback = [this](int index) {
            _cardNodes.at(index)->removeFromParent();
            _cardNodes.erase(_cardNodes.begin() + index);
            reload();
        };
        
        if (_cardNodes.size() > index) {
            auto node = _cardNodes.at(index);
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

void MapUICardDeck::reload()
{
    const size_t maxCnt(_unitPositions.size());
    const size_t cnt(_cardNodes.size());
    for (int i = 0; i < cnt; ++i) {
        auto node = _cardNodes.at(i);
        if (i < maxCnt) {
            const Point& position = _unitPositions.at(i);
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
        node->setPosition(_unitPositions[i++]);
        _cardNodes.push_back(node);
        _buffers.pop();
    }
}

#pragma mark shake
static float shake_action_tag = 2016;
void MapUICardDeck::shake()
{
    if (!_isShaking) {
        stopShake();
        _isShaking = true;
        
        static float shake_duration = 0.4f;
        static float shake_strength = 2.0f;
        
        auto action = RepeatForever::create(CCShake::actionWithDuration(shake_duration, shake_strength, _candidateSpritePosition));
        action->setTag(shake_action_tag);
        _candidateSprite->runAction(action);
    }
}

void MapUICardDeck::stopShake()
{
    if (_isShaking) {
        _isShaking = false;
    }
    
    if (_candidateSprite->getActionByTag(shake_action_tag)) {
        _candidateSprite->stopActionByTag(shake_action_tag);
        _candidateSprite->setPosition(_candidateSpritePosition);
    }
}
