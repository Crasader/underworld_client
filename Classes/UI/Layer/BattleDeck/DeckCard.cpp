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

DeckCard* DeckCard::create(const AbstractData* data)
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
,_qualityBox(nullptr)
,_data(nullptr)
,_touchInvalid(false)
,_originalPoint(Point::ZERO) {}

DeckCard::~DeckCard()
{
    removeAllChildren();
}

bool DeckCard::init(const AbstractData* data)
{
    if (Widget::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(Size(Width, Height));
        
        auto box = Sprite::create(CocosUtils::getResourcePath("ui_quality_box_blue.png"));
        box->setPosition(Width / 2, Height / 2);
        addChild(box);
        _qualityBox = box;
        _originalPoint = box->getPosition();
        
        _icon = Sprite::create("GameImages/icons/unit/icon_fashi.png");
        _icon->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        _icon->setPosition(box->getContentSize().width / 2, 8);
        box->addChild(_icon);
        
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
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, [this](Ref*, ui::Widget::TouchEventType type) {
            if (_observer) {
                _observer->onDeckCardTouched(this, type);
            }

        }, [this](Ref*) {
            if (_observer) {
                _observer->onDeckCardClicked(this);
            }
        });
        
        update(data);
        
        return true;
    }
    
    return false;
}

void DeckCard::registerObserver(DeckCardObserver *observer)
{
    _observer = observer;
}

void DeckCard::update(const AbstractData* data)
{
    if (_data != data) {
        _data = data;
        
        const int cardId(getCardId());
        auto cd(DataManager::getInstance()->getCardConfigData(cardId));
        if (cd && _icon) {
            _icon->setTexture(cd->getIcon());
        }
        
        auto csData(dynamic_cast<const CardSimpleData*>(data));
        if (csData) {
            _costNode->setVisible(data ? !csData->isHero() : false);
            _cost->setString(data ? StringUtils::format("%d", csData->getCost()) : "");
        } else {
            _cost->setVisible(false);
        }
        
        if (false) {
            _qualityBox->setTexture(CocosUtils::getResourcePath("ui_quality_box_blue.png"));
        }
    }
}

const AbstractData* DeckCard::getCardData() const
{
    return _data;
}

int DeckCard::getCardId() const
{
    return _data ? _data->getId() : 0;
}

static float shake_action_tag = 2016;
void DeckCard::shake()
{
    static float shake_duration = 0.2f;
    static float shake_strength = 1.0f;
    
    auto action = RepeatForever::create(CCShake::actionWithDuration(shake_duration, shake_strength, _originalPoint));
    action->setTag(shake_action_tag);
    _qualityBox->runAction(action);
}

void DeckCard::stopShake()
{
    if (_qualityBox->getActionByTag(shake_action_tag)) {
        _qualityBox->stopActionByTag(shake_action_tag);
        _qualityBox->setPosition(_originalPoint);
    }
}
