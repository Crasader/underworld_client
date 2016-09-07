//
//  JuniorCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "JuniorCard.h"
#include "CocosUtils.h"
#include "ObjectUtils.h"
#include "AbstractData.h"
#include "CardUpgradeProperty.h"
#include "SkillUpgradeProperty.h"

using namespace std;

JuniorCard* JuniorCard::create()
{
    auto ret = new (nothrow) JuniorCard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

JuniorCard::JuniorCard()
:_ptBackground(nullptr)
,_pt(nullptr)
,_amount(nullptr)
,_arrow(nullptr) {}

JuniorCard::~JuniorCard()
{
    removeAllChildren();
}

bool JuniorCard::init()
{
    if (BaseCard::init()) {
        auto ptbg = Sprite::create(CocosUtils::getResourcePath("ui_tiao_27.png"));
        addChild(ptbg);
        _ptBackground = ptbg;
        
        const auto& psize(ptbg->getContentSize());
        auto pt = ProgressTimer::create(Sprite::create(CocosUtils::getResourcePath("ui_tiao_28.png")));
        pt->setType(ProgressTimer::Type::BAR);
        pt->setBarChangeRate(Vec2(1.0f, 0.0f));
        pt->setMidpoint(Vec2::ANCHOR_BOTTOM_LEFT);
        pt->setPercentage(80.f);
        pt->setPosition(psize.width / 2, psize.height / 2);
        ptbg->addChild(pt);
        _pt = pt;
        
        auto label = CocosUtils::createLabel("20/100", DEFAULT_FONT_SIZE);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setTextColor(Color4B::WHITE);
        label->setPosition(psize.width / 2, psize.height / 2);
        ptbg->addChild(label);
        _amount = label;
        
        static const float space(5);
        const Size originalSize(getContentSize());
        const Size size(originalSize + Size(0, space + psize.height));
        setContentSize(size);
        
        resetPositions(size - originalSize);
        ptbg->setPosition(Point(size.width / 2, psize.height / 2));
        
        auto arrow = Sprite::create(CocosUtils::getResourcePath("icon_jiantou_4.png"));
        arrow->setPosition(0, psize.height / 2);
        ptbg->addChild(arrow);
        _arrow = arrow;
        
        return true;
    }
    
    return false;
}

void JuniorCard::updateData(const AbstractData* data)
{
    BaseCard::updateData(data);
    
    const bool show(nullptr != data);
    if (_ptBackground) {
        _ptBackground->setVisible(show);
    }
    
    if (show) {
        int total(0);
        switch (ObjectUtils::getType(data->getId())) {
            case ObjectUtils::Type::CARD: {
                auto up(dynamic_cast<const CardUpgradeProperty*>(data->getUpgradeProperty()));
                if (up) {
                    total = up->getCardCost();
                }
            }
                break;
            case ObjectUtils::Type::SKILL: {
                auto up(dynamic_cast<const SkillUpgradeProperty*>(data->getUpgradeProperty()));
                if (up) {
                    total = up->getBookCost().second;
                }
            }
            default:
                break;
        }
        
        // TODO: for skill, the amount is the book count
        const auto amount(data->getAmount());
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
}

bool JuniorCard::canUpgrade() const
{
    return _pt->getPercentage() >= 100.0f;
}
