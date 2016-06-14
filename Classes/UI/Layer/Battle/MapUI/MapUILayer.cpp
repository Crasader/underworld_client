//
//  MapUILayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUILayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "UnitCardDeck.h"
#include "BattleResourceNode.h"
#include "SoundManager.h"

using namespace std;
using namespace UnderWorld::Core;

static ProgressTimer* createProgressTimer(const string& file)
{
    Sprite* s = Sprite::create(file);
    ProgressTimer* pt = ProgressTimer::create(s);
    pt->setType(ProgressTimer::Type::BAR);
    pt->setBarChangeRate(Vec2(1.0f, 0.0f));
    pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
    
    return pt;
}

MapUILayer* MapUILayer::create(const string& myAccount, const string& opponentsAccount)
{
    MapUILayer *ret = new (nothrow) MapUILayer();
    if (ret && ret->init(myAccount, opponentsAccount))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MapUILayer::MapUILayer()
:_observer(nullptr)
,_isTouchingTableView(false)
,_decksTotalWidth(0)
,_timeLabel(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_pauseMenuItem(nullptr)
,_resourceNode(nullptr)
{
    clearCardInfo(_highlightedCardInfo);
    clearCardInfo(_selectedCardInfo);
}

MapUILayer::~MapUILayer()
{
    removeAllChildren();
}

void MapUILayer::registerObserver(MapUILayerObserver *observer)
{
    _observer = observer;
}

void MapUILayer::reload()
{
    
}

void MapUILayer::updateMyHpProgress(int progress)
{
    if (_myHpProgress) {
        _myHpProgress->setPercentage(progress);
    }
    
    if (_myHpPercentageLabel) {
        progress = MAX(0, MIN(progress, 100));
        _myHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
    }
}

void MapUILayer::updateOpponentsHpProgress(int progress)
{
    if (_opponentsHpProgress) {
        _opponentsHpProgress->setPercentage(progress);
    }
    
    if (_opponentsHpPercentageLabel) {
        progress = MAX(0, MIN(progress, 100));
        _opponentsHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
    }
}

void MapUILayer::updateRemainingTime(int time)
{
    if (_timeLabel) {
        _timeLabel->setString(CocosUtils::getFormattedTime(time));
        
        if (time <= 180) {
            _timeLabel->setTextColor(Color4B::RED);
        } else {
            _timeLabel->setTextColor(Color4B::WHITE);
        }
    }
}

void MapUILayer::pauseGame()
{
    
}

void MapUILayer::resumeGame()
{
    
}

void MapUILayer::clearHighlightedCard()
{
    setHighlightedCard(nullptr, INVALID_VALUE);
}

bool MapUILayer::isPointInDeck(const Point& point) const
{
    for (auto iter = begin(_decks); iter != end(_decks); ++iter) {
        auto deck = iter->second;
        if (deck && deck->getBoundingBox().containsPoint(point)) {
            return true;
        }
    }
    
    return false;
}

#pragma mark - card deck
void MapUILayer::createCardDeck(CardDeckType type, int count)
{
    if (count > 0 && (_decks.find(type) == end(_decks))) {
        CardDeck* deck(nullptr);
        if (CardDeckType::Unit == type) {
            deck = UnitCardDeck::create(count);
        } else {
            deck = CardDeck::create(count);
        }
        
        if (deck) {
            deck->registerObserver(this);
            addChild(deck);
            _decks.insert(make_pair(type, deck));
            
            // reset positions
            _decksTotalWidth += deck->getContentSize().width;
            reorderDecks();
        }
    }
}

void MapUILayer::insertCard(CardDeckType type, const string& name)
{
    auto deck = getDeck(type);
    if (deck) {
        deck->insert(name, false);
    }
}

void MapUILayer::insertCard(CardDeckType type, const HMMCard* card)
{
    auto deck = getDeck(type);
    if (deck) {
        deck->insert(card, true);
    }
}

void MapUILayer::removeCard(CardDeckType type, const HMMCard* card, int index)
{
    auto deck = getDeck(type);
    if (deck) {
        deck->remove(card, index, true);
    }
}

void MapUILayer::updateNextCard(const HMMCard* card)
{
    static CardDeckType type(CardDeckType::Unit);
    auto deck = dynamic_cast<UnitCardDeck*>(getDeck(type));
    if (deck) {
        deck->updateNextCard(card);
    }
}

void MapUILayer::updateCardCD(CardDeckType type, int idx, float percentage)
{
    auto deck = getDeck(type);
    if (deck) {
        deck->updateCD(idx, percentage);
    }
}

void MapUILayer::updateCountDown(float time, float duration)
{
    static CardDeckType type(CardDeckType::Unit);
    auto deck = dynamic_cast<UnitCardDeck*>(getDeck(type));
    if (deck) {
        deck->updateTimer(time, duration);
    }
}

void MapUILayer::updateResource(const unordered_map<string, float>& resources)
{
    static CardDeckType type(CardDeckType::Unit);
    auto deck = dynamic_cast<UnitCardDeck*>(getDeck(type));
    if (deck) {
        deck->updateResource(resources);
    }
    
    for (auto iter = begin(resources); iter != end(resources); ++iter) {
        const string& key = iter->first;
        if (_resourceLabels.find(key) != end(_resourceLabels)) {
            _resourceLabels.at(key)->setString(StringUtils::format("%.0f", iter->second));
        }
    }
}

#pragma mark -
bool MapUILayer::init(const string& myAccount, const string& opponentsAccount)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
#if false
        // yezi
        {
            static const string file("particle/yezi/yezi.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 - 10));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 + 10));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0_0_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 + 20));
            addChild(effect);
        }
#endif
        
#if false
        static const string CsbFile("rankInfo_UI.csb");
        Node *mainNode = CocosUtils::playCSBAnimation(CsbFile, false, 0, nullptr);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        Widget* root = dynamic_cast<Widget *>(mainNode->getChildByTag(777));
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                            
                    }
                }
            }
        }
#else
        Node* root = this;
        static const float ceilOffset(5.0f);
        static const Size progressSize(170.0f, 20.0f);
        
        createUserInfo(true, myAccount);
        
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_time.png");
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            sprite->setPosition(Point(winSize.width / 2, winSize.height - size.height / 2 - ceilOffset));
            
            _timeLabel = CocosUtils::createLabel("", 24, DEFAULT_NUMBER_FONT);
            _timeLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            _timeLabel->setPosition(Point(size.width * 0.45f, size.height / 2));
            sprite->addChild(_timeLabel);
        }
        
        createUserInfo(false, opponentsAccount);
        
        // buttons
        {
            _pauseMenuItem = MenuItemImage::create("GameImages/test/ui_zt.png", "GameImages/test/ui_zt.png", [this](Ref*) {
                if (_observer) {
                    _observer->onMapUILayerClickedPauseButton();
                }
            });
            _pauseMenuItem->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
            _pauseMenuItem->setPosition(Point(winSize.width - 5.0f, winSize.height - 5.0f));
            Menu *menu = Menu::create(_pauseMenuItem, nullptr);
            menu->setPosition(Point::ZERO);
            root->addChild(menu);
        }
#endif
        
#if false
        addResourceNode();
#endif
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MapUILayer::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(MapUILayer::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MapUILayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool MapUILayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    const Point& point = touch->getLocation();
    if (isPointInDeck(point)) {
        _isTouchingTableView = true;
        return true;
    }
    
    return false;
}

void MapUILayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    const bool gameOver = isGameOver();
    if (!gameOver && _isTouchingTableView && _selectedCard.length() > 0) {
        const Point& point = touch->getLocation();
        const bool inDeck = isPointInDeck(point);
        
        if (_observer) {
            _observer->onMapUILayerTouchMoved(_selectedCard, point);
        }
        
        if (!inDeck && _highlightedCardInfo != _selectedCardInfo) {
            setHighlightedCard(_selectedCardInfo.first, _selectedCardInfo.second);
        }
    }
}

void MapUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    const bool gameOver = isGameOver();
    if (!gameOver && _isTouchingTableView && _selectedCard.length() > 0) {
        if (_observer) {
            const Point& point = touch->getLocation();
            _observer->onMapUILayerTouchEnded(_selectedCard, _selectedCardInfo.second, point);
        }
        
        clearCardInfo(_selectedCardInfo);
        _selectedCard = "";
        _isTouchingTableView = false;
    }
}

#pragma mark - UnitCardDeckObserver
void MapUILayer::onCardDeckTouchedBegan(CardDeck* node, const string& card, int idx)
{
    setCardInfo(_selectedCardInfo, node, idx);
    _selectedCard = card;
}

void MapUILayer::onCardDeckTouchedEnded(CardDeck* node, const string& card, int idx)
{
    if (!isGameOver()) {
        setHighlightedCard(node, idx);
        SoundManager::getInstance()->playButtonSelectUnitSound();
    }
}

#pragma mark private
void MapUILayer::createUserInfo(bool left, const string& account)
{
    const Size& winSize = Director::getInstance()->getWinSize();
    
    Sprite* nameBg = CocosUtils::createPureColorSprite(Size(163, 26), LAYER_MASK_COLOR);
    addChild(nameBg);
    
    const Size& nameBgSize = nameBg->getContentSize();
    static const float offsetX(192.0f);
    nameBg->setPosition(Point(winSize.width / 2 + offsetX * (left ? -1 : 1), winSize.height - nameBgSize.height / 2));
    
    const Point& position = nameBg->getPosition();
    
    // name
    {
        Label* label = CocosUtils::createLabel(account, DEFAULT_FONT_SIZE);
        label->setPosition(Point(nameBgSize.width / 2, nameBgSize.height / 2));
        nameBg->addChild(label);
    }
    
    // progress
    {
        Sprite* sprite = Sprite::create(left ? "GameImages/test/ui_blood_left_bg.png" : "GameImages/test/ui_blood_right_bg.png");
        addChild(sprite);
        
        const Size& size = sprite->getContentSize();
        sprite->setPosition(position - Point(0, (nameBgSize.height + size.height) / 2));
        
        ProgressTimer* pt = createProgressTimer(left ? "GameImages/test/ui_blood_left.png" : "GameImages/test/ui_blood_right.png");
        pt->setPosition(Point(size.width / 2, size.height / 2));
        pt->setMidpoint(left ? Point::ANCHOR_BOTTOM_RIGHT : Point::ANCHOR_BOTTOM_LEFT);
        sprite->addChild(pt);
        
        Label* ptLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        ptLabel->setPosition(pt->getPosition());
        sprite->addChild(ptLabel);
        
        if (left) {
            _myHpProgress = pt;
            _myHpPercentageLabel = ptLabel;
        } else {
            _opponentsHpProgress = pt;
            _opponentsHpPercentageLabel = ptLabel;
        }
    }
    
    // icon
    {
        Sprite* sprite = Sprite::create(left ? "GameImages/test/icon_zg.png" : "GameImages/test/icon_bc.png");
        addChild(sprite);
        
        const Size& size = sprite->getContentSize();
        sprite->setPosition(Point(position.x + (nameBgSize.width / 2 + size.width / 2) * (left ? 1 : -1), winSize.height - size.height / 2));
    }
}

bool MapUILayer::isGameOver() const
{
    if (_observer) {
        return _observer->onMapUILayerIsGameOver();
    }
    
    return false;
}

void MapUILayer::reorderDecks()
{
#if false
    if (_resourceNode) {
        static const float offsetX(10.0f);
        float x(_resourceNode->getPosition().x + _resourceNode->getContentSize().width / 2);
        
        for (auto iter = begin(_decks); iter != end(_decks); ++iter) {
            auto deck = iter->second;
            if (deck) {
                const Size& size = deck->getContentSize();
                x += offsetX + size.width;
                deck->setPosition(x - size.width / 2, size.height / 2);
            }
        }
    }
#else
    const float winWidth = Director::getInstance()->getWinSize().width;
    const size_t cnt(_decks.size());
    if (winWidth >= _decksTotalWidth) {
        const float offsetX = (winWidth - _decksTotalWidth) / (cnt + 1);
        float x(0);
        for (auto iter = begin(_decks); iter != end(_decks); ++iter) {
            auto deck = iter->second;
            if (deck) {
                const Size& size = deck->getContentSize();
                x += offsetX + size.width;
                deck->setPosition(x - size.width / 2, size.height / 2);
            }
        }
    } else {
        // TODO
    }
#endif
}

CardDeck* MapUILayer::getDeck(CardDeckType type) const
{
    if (_decks.find(type) != end(_decks)) {
        return _decks.at(type);
    }
    
    return nullptr;
}

void MapUILayer::setHighlightedCard(CardDeck* deck, int idx)
{
    const bool isNew = (_highlightedCardInfo.first != deck || _highlightedCardInfo.second != idx);
    
    if (isNew) {
        setCardInfo(_highlightedCardInfo, deck, idx);
    } else {
        clearCardInfo(_highlightedCardInfo);
    }
    
    for (auto iter = begin(_decks); iter != end(_decks); ++iter) {
        auto d = iter->second;
        d->select((deck == d) ? _highlightedCardInfo.second : INVALID_VALUE);
    }
    
    if (_observer) {
        if (deck) {
            const int highlightedIdx(_highlightedCardInfo.second);
            const string& card = deck->getCard(highlightedIdx);
            _observer->onMapUILayerCardSelected(card, highlightedIdx);
        } else {
            _observer->onMapUILayerCardSelected("", INVALID_VALUE);
        }
    }
}

void MapUILayer::setCardInfo(pair<CardDeck*, int>& data, CardDeck* deck, int idx) const
{
    data.first = deck;
    data.second = idx;
}

void MapUILayer::clearCardInfo(pair<CardDeck*, int>& data) const
{
    setCardInfo(data, nullptr, INVALID_VALUE);
}

void MapUILayer::addResourceNode()
{
    if (!_resourceNode) {
        auto node = Node::create();
        addChild(node);
        node->setAnchorPoint(Point::ANCHOR_MIDDLE);
        static const Size size(99, MAP_OFFSET_Y);
        node->setContentSize(size);
        node->setPosition(size.width / 2, size.height / 2);
        
        auto line = Sprite::create("GameImages/test/ui_line.png");
        line->setScaleX(size.width / 2);
        line->setPosition(size.width / 2, size.height / 2);
        node->addChild(line);
        
        {
            auto label = CocosUtils::createLabel("0", 36, DEFAULT_NUMBER_FONT);
            label->setPosition(size.width / 2, size.height * 3 / 4);
            node->addChild(label);
            _resourceLabels.insert(make_pair(RES_NAME_GOLD, label));
        }
        
        {
            auto label = CocosUtils::createLabel("0", 36, DEFAULT_NUMBER_FONT);
            label->setPosition(size.width / 2, size.height / 4);
            node->addChild(label);
            _resourceLabels.insert(make_pair(RES_NAME_WOOD, label));
        }
        
        _resourceNode = node;
    }
}
