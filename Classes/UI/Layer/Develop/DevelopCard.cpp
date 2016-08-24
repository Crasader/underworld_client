//
//  DevelopCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DevelopCard.h"
#include "CocosUtils.h"
#include "DevelopUI.h"
#include "DeckCard.h"
#include "DeckManager.h"
#include "CardSimpleData.h"

using namespace std;

DevelopCard* DevelopCard::create(int cardId)
{
    auto ret = new (nothrow) DevelopCard();
    if (ret && ret->init(cardId)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DevelopCard::DevelopCard()
:_observer(nullptr)
,_icon(nullptr)
,_touchInvalid(false)
,_pt(nullptr)
,_amount(nullptr) {}

DevelopCard::~DevelopCard()
{
    removeAllChildren();
}

bool DevelopCard::init(int cardId)
{
    if (ui::Widget::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setTouchEnabled(true);
        setSwallowTouches(false);
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, [this](Ref*) {
            if (_observer) {
                _observer->onDevelopCardClicked(this, _pt->getPercentage() >= 100);
            }
        });
        
        _icon = DeckCard::create(cardId);
        addChild(_icon);
        
        auto ptbg = Sprite::create(DevelopUI::getResourcePath("ui_tiao_27.png"));
        addChild(ptbg);
        
        auto pt = ProgressTimer::create(Sprite::create(DevelopUI::getResourcePath("ui_tiao_28.png")));
        pt->setType(ProgressTimer::Type::BAR);
        pt->setBarChangeRate(Vec2(1.0f, 0.0f));
        pt->setMidpoint(Vec2::ANCHOR_BOTTOM_LEFT);
        pt->setPercentage(80.f);
        ptbg->addChild(pt);
        _pt = pt;
        
        const auto& psize(ptbg->getContentSize());
        pt->setPosition(psize.width / 2, psize.height / 2);
        
        auto label = CocosUtils::createLabel("20/100", DEFAULT_FONT_SIZE);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setTextColor(Color4B::BLACK);
        label->setPosition(psize.width / 2, psize.height / 2);
        ptbg->addChild(label);
        _amount = label;
        
        static const float space(5);
        const Size size(DeckCard::Width, DeckCard::Height + space + psize.height);
        setContentSize(size);
        
        _icon->setPosition(Point(size.width / 2, size.height - DeckCard::Height / 2));
        ptbg->setPosition(Point(size.width / 2, psize.height / 2));
        
        updateAmount(cardId);
        
        return true;
    }
    
    return false;
}

void DevelopCard::registerObserver(DevelopCardObserver *observer)
{
    _observer = observer;
}

void DevelopCard::update(int cardId)
{
    if (_icon) {
        _icon->update(cardId);
    }
    
    updateAmount(cardId);
}

int DevelopCard::getCardId() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}


void DevelopCard::updateAmount(int cardId)
{
    auto data = DeckManager::getInstance()->getCardData(cardId);
    if (data) {
        const auto amount(data->getAmount());
        if (_pt) {
            _pt->setPercentage(100.0f * amount / 100.0f);
        }
        
        if (_amount) {
            _amount->setString(StringUtils::format("%d/%d", amount, 100));
        }
    }
}
