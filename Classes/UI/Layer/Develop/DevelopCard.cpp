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
#include "DataManager.h"
#include "AbstractData.h"
#include "CardUpgradeProperty.h"
#include "SkillUpgradeProperty.h"

using namespace std;

DevelopCard* DevelopCard::create(const AbstractData* data)
{
    auto ret = new (nothrow) DevelopCard();
    if (ret && ret->init(data)) {
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
,_amount(nullptr)
,_arrow(nullptr) {}

DevelopCard::~DevelopCard()
{
    removeAllChildren();
}

bool DevelopCard::init(const AbstractData* data)
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
        
        _icon = DeckCard::create(0);
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
        label->setTextColor(Color4B::WHITE);
        label->setPosition(psize.width / 2, psize.height / 2);
        ptbg->addChild(label);
        _amount = label;
        
        static const float space(5);
        const Size size(DeckCard::Width, DeckCard::Height + space + psize.height);
        setContentSize(size);
        
        _icon->setPosition(Point(size.width / 2, size.height - DeckCard::Height / 2));
        ptbg->setPosition(Point(size.width / 2, psize.height / 2));
        
        auto arrow = Sprite::create(DevelopUI::getResourcePath("icon_jiantou_4.png"));
        arrow->setPosition(ptbg->getPosition() - Point(psize.width / 2, 0));
        addChild(arrow);
        _arrow = arrow;
        
        update(data);
        
        return true;
    }
    
    return false;
}

void DevelopCard::registerObserver(DevelopCardObserver *observer)
{
    _observer = observer;
}

void DevelopCard::update(const AbstractData* data)
{
    if (_icon) {
        _icon->update(data);
    }
    
    int total(0);
    if (data) {
        auto cardUp(DataManager::getInstance()->getCardUpgradeProperty(data->getId(), data->getLevel()));
        if (cardUp) {
            total = cardUp->getCardCost();
        } else {
            auto skillUp(DataManager::getInstance()->getSkillUpgradeProperty(data->getId(), data->getLevel()));
            if (skillUp) {
                total = skillUp->getBookCost().second;
            }
        }
    }
    
    const auto amount(data ? data->getAmount() : 0);
    if (_pt) {
        _pt->setPercentage(100.0f * amount / total);
    }
    
    if (_amount) {
        _amount->setString(StringUtils::format("%d/%d", amount, total));
    }
    
    if (_arrow) {
        _arrow->setVisible(amount >= total);
    }
}

bool DevelopCard::canUpgrade() const
{
    return _pt->getPercentage() >= 100.0f;
}

const AbstractData* DevelopCard::getCardData() const
{
    if (_icon) {
        return _icon->getCardData();
    }
    
    return nullptr;
}
