//
//  DeckCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DeckCard.h"
#include "CocosUtils.h"
#include "BattleDeckUI.h"
#include "DataManager.h"
#include "CardSimpleData.h"
#include "CCShake.h"

using namespace std;

DeckCard* DeckCard::create(const CardSimpleData* data)
{
    auto ret = new (nothrow) DeckCard();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DeckCard::DeckCard()
:_observer(nullptr)
,_icon(nullptr)
,_costNode(nullptr)
,_cost(nullptr)
,_data(nullptr)
,_isHero(false)
,_inDeck(false)
,_touchInvalid(false)
,_iconPoint(Point::ZERO) {}

DeckCard::~DeckCard()
{
    removeAllChildren();
}

bool DeckCard::init(const CardSimpleData* data)
{
    if (Widget::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(Size(Width, Height));
        
        _icon = Sprite::create("GameImages/icons/unit/icon_fashi.png");
        _icon->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        _icon->setPosition(Width / 2, 0);
        _iconPoint = _icon->getPosition();
        addChild(_icon);
        
        static const float offsetY(4);
        _costNode = Sprite::create(CocosUtils::getResourcePath("ui_qiu.png"));
        _costNode->setPosition(Width / 2, Height - offsetY);
        _icon->addChild(_costNode);
        
        _cost = CocosUtils::createLabel("1", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _cost->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        _cost->setAnchorPoint(Point::ANCHOR_MIDDLE);
        const auto& csize(_costNode->getContentSize());
        _cost->setPosition(csize.width / 2, csize.height / 2 + offsetY / 2);
        _costNode->addChild(_cost);
        
        setTouchEnabled(true);
        setSwallowTouches(false);
        addTouchEventListener([this](Ref *pSender, ui::Widget::TouchEventType type) {
            if (_observer) {
                _observer->onDeckCardTouched(this, type);
            }
            
            auto widget = dynamic_cast<ui::Widget*>(pSender);
            if (type == ui::Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
            } else if (type == ui::Widget::TouchEventType::MOVED) {
                if (!_touchInvalid) {
                    const auto& mp(widget->getTouchMovePosition());
                    const auto& bp(widget->getTouchBeganPosition());
                    static const float offset(40);
                    if (abs(mp.x - bp.x) >= offset || abs(mp.y - bp.y) >= offset) {
                        _touchInvalid = true;
                    }
                }
            } else if (type == ui::Widget::TouchEventType::ENDED) {
                if (!_touchInvalid && _observer) {
                    _observer->onDeckCardClicked(this);
                }
            }
        });
        
        return true;
    }
    
    return false;
}

void DeckCard::registerObserver(DeckCardObserver *observer)
{
    _observer = observer;
}

void DeckCard::update(const CardSimpleData* data)
{
    if (_data != data) {
        _data = data;
    }
}

// TODO: remove this method
void DeckCard::setIsHero(bool isHero)
{
    if (_isHero != isHero) {
        _isHero = isHero;
        static const string file("GameImages/icons/unit/");
        _icon->setTexture(file + (isHero ? "icon_shirenmo.png" : "icon_fashi.png"));
        _costNode->setVisible(!isHero);
    }
}

void DeckCard::setInDeck(bool inDeck)
{
    if (_inDeck != inDeck) {
        _inDeck = inDeck;
    }
}

const CardSimpleData* DeckCard::getData() const
{
    return _data;
}

static float shake_action_tag = 2016;
void DeckCard::shake()
{
    static float shake_duration = 0.2f;
    static float shake_strength = 1.0f;
    
    auto action = RepeatForever::create(CCShake::actionWithDuration(shake_duration, shake_strength, _iconPoint));
    action->setTag(shake_action_tag);
    _icon->runAction(action);
}

void DeckCard::stopShake()
{
    if (_icon->getActionByTag(shake_action_tag)) {
        _icon->stopActionByTag(shake_action_tag);
        _icon->setPosition(_iconPoint);
    }
}
