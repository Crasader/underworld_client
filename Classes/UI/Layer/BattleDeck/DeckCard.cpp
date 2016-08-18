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
#include "DeckManager.h"
#include "DataManager.h"
#include "CardConfigData.h"
#include "CardSimpleData.h"
#include "CCShake.h"

using namespace std;

DeckCard* DeckCard::create(int cardId)
{
    auto ret = new (nothrow) DeckCard();
    if (ret && ret->init(cardId)) {
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
,_cardId(0)
,_touchInvalid(false)
,_iconPoint(Point::ZERO) {}

DeckCard::~DeckCard()
{
    removeAllChildren();
}

bool DeckCard::init(int cardId)
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
        
        update(cardId);
        
        return true;
    }
    
    return false;
}

void DeckCard::registerObserver(DeckCardObserver *observer)
{
    _observer = observer;
}

void DeckCard::update(int cardId)
{
    if (_cardId != cardId) {
        _cardId = cardId;
        
        auto data = DeckManager::getInstance()->getCardData(cardId);
        if (data) {
            auto cd(DataManager::getInstance()->getCardConfigData(data->getIdx()));
            if (cd && _icon) {
                _icon->setTexture(cd->getIcon());
            }
            
            _costNode->setVisible(!data->isHero());
            _cost->setString(StringUtils::format("%d", data->getCost()));
        }
    }
}

int DeckCard::getCardId() const
{
    return _cardId;
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
