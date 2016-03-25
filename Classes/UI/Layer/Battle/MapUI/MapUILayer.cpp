//
//  MapUILayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUILayer.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "Deck.h"
#include "BattleResourceNode.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
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
,_highlightedCard(INVALID_VALUE)
,_timeLabel(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_pauseMenuItem(nullptr)
,_cardDeck(nullptr)
{
    setHighlightedCard(INVALID_VALUE);
    _selectedCard.first = nullptr;
    _selectedCard.second = INVALID_VALUE;
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
    setHighlightedCard(INVALID_VALUE);
}

bool MapUILayer::isPointInTableView(const Point& point)
{
    if (_cardDeck && _cardDeck->getBoundingBox().containsPoint(point)) {
        return true;
    }
    
    return false;
}

#pragma mark - card deck
void MapUILayer::createCardDeck(int count)
{
    if (count > 0) {
        _cardDeck = MapUICardDeck::create(count);
        _cardDeck->registerObserver(this);
        addChild(_cardDeck);
        
        const Size& winSize = Director::getInstance()->getWinSize();
        const Size& size = _cardDeck->getContentSize();
        _cardDeck->setPosition(winSize.width / 2, size.height / 2);
    }
}

void MapUILayer::insertCard(const Card* card)
{
    if (_cardDeck) {
        _cardDeck->insert(card, true);
    }
}

void MapUILayer::removeCard(const Card* card, int index)
{
    if (_cardDeck) {
        _cardDeck->remove(card, index, true);
    }
}

void MapUILayer::updateCardDeckCountDown(float time, float duration)
{
    if (_cardDeck) {
        _cardDeck->updateTimer(time, duration);
    }
}

void MapUILayer::updateCardDeckResource(const map<string, float>& resources)
{
    if (_cardDeck) {
        _cardDeck->updateResource(resources);
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
        Node *mainNode = CSLoader::createNode(CsbFile);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
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
    if (isPointInTableView(point)) {
        _isTouchingTableView = true;
        return true;
    }
    
    return false;
}

void MapUILayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    const bool gameOver = isGameOver();
    if (!gameOver && _isTouchingTableView && _selectedCard.first) {
        const Point& point = touch->getLocation();
        const bool inDeck = isPointInTableView(point);
        
        if (_observer) {
            _observer->onMapUILayerTouchMoved(_selectedCard.first, point);
        }
        
        if (!inDeck && _highlightedCard != _selectedCard.second) {
            setHighlightedCard(_selectedCard.second);
        }
    }
}

void MapUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    const bool gameOver = isGameOver();
    if (!gameOver && _isTouchingTableView && _selectedCard.first) {
        if (_observer) {
            const Point& point = touch->getLocation();
            _observer->onMapUILayerTouchEnded(_selectedCard.first, _selectedCard.second, point);
        }
        
        _selectedCard.first = nullptr;
        _selectedCard.second = INVALID_VALUE;
        _isTouchingTableView = false;
    }
}

#pragma mark - MapUICardDeckObserver
void MapUILayer::onMapUICardDeckUnitTouchedBegan(const Card* card, int idx)
{
    _selectedCard.first = card;
    _selectedCard.second = idx;
}

void MapUILayer::onMapUICardDeckUnitTouchedEnded(const Card* card, int idx)
{
    if (!isGameOver()) {
        setHighlightedCard(idx);
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

void MapUILayer::setHighlightedCard(int idx)
{
    const int lastCard = _highlightedCard;
    const bool isNew = (lastCard != idx);
    
    if (isNew) {
        _highlightedCard = idx;
    } else {
        _highlightedCard = INVALID_VALUE;
    }
    
    if (_cardDeck) {
        _cardDeck->select(_highlightedCard);
        
        // callback
        if (_observer) {
            const Card* card = _cardDeck->getCard(_highlightedCard);
            _observer->onMapUILayerCardSelected(card, _highlightedCard);
        }
    }
}
