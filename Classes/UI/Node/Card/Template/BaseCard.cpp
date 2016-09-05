//
//  BaseCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BaseCard.h"
#include "CocosUtils.h"
#include "BattleDeckUI.h"
#include "DeckManager.h"
#include "DataManager.h"
#include "CardConfigData.h"
#include "AbstractData.h"
#include "CardProperty.h"
#include "CCShake.h"

using namespace std;

BaseCard* BaseCard::create()
{
    auto ret = new (nothrow) BaseCard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BaseCard::BaseCard()
:_observer(nullptr)
,_costNode(nullptr)
,_cost(nullptr)
,_infoButton(nullptr)
,_touchInvalid(false)
,_originalPoint(Point::ZERO) {}

BaseCard::~BaseCard()
{
    removeAllChildren();
}

bool BaseCard::init()
{
    if (AbstractCard::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(Size(Width, Height));
        
        auto box = Sprite::create(CocosUtils::getResourcePath("ui_quality_box_blue.png"));
        box->setPosition(Width / 2, Height / 2);
        addChild(box);
        _qualityBox = box;
        _originalPoint = box->getPosition();
        
        auto card = Sprite::create("GameImages/icons/unit/icon_fashi.png");
        card->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        card->setPosition(box->getContentSize().width / 2, 8);
        box->addChild(card);
        _icon = card;
        
        static const float offsetY(4);
        auto costNode = Sprite::create(CocosUtils::getResourcePath("ui_qiu.png"));
        costNode->setPosition(Width / 2, Height - offsetY);
        card->addChild(costNode);
        _costNode = costNode;
        
        const auto& csize(costNode->getContentSize());
        auto costLabel = CocosUtils::createLabel("1", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        costLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        costLabel->setAnchorPoint(Point::ANCHOR_MIDDLE);
        costLabel->setPosition(csize.width / 2, csize.height / 2 + offsetY / 2);
        costNode->addChild(costLabel);
        _cost = costLabel;
        
        auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        label->setTextColor(Color4B::ORANGE);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setPosition(Width / 2, label->getContentSize().height / 2 + offsetY);
        card->addChild(label);
        _level = label;
        
        static const string file(CocosUtils::getResourcePath("button_lanse_3.png"));
        auto info = ui::Button::create(file, file);
        info->addClickEventListener([this](Ref*) {
            if (_observer) {
                _observer->onBaseCardInfo(this);
            }
        });
        static const float iedge(5);
        const auto& isize(info->getContentSize());
        info->setPosition(Point(Width - (isize.width / 2 + iedge), Height - (isize.height / 2 + iedge)));
        info->setVisible(false);
        addChild(info);
        _infoButton = info;
        
        setTouchEnabled(true);
        setSwallowTouches(false);
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, [this](Ref*, ui::Widget::TouchEventType type) {
            if (_observer) {
                _observer->onBaseCardTouched(this, type);
            }

        }, [this](Ref*) {
            if (_observer) {
                _observer->onBaseCardClicked(this);
            }
        });
        
        return true;
    }
    
    return false;
}

void BaseCard::registerObserver(BaseCardObserver *observer)
{
    _observer = observer;
}

static float shake_action_tag = 2016;
void BaseCard::shake()
{
    static float shake_duration = 0.2f;
    static float shake_strength = 1.0f;
    
    auto action = RepeatForever::create(CCShake::actionWithDuration(shake_duration, shake_strength, _originalPoint));
    action->setTag(shake_action_tag);
    _qualityBox->runAction(action);
}

void BaseCard::stopShake()
{
    if (_qualityBox->getActionByTag(shake_action_tag)) {
        _qualityBox->stopActionByTag(shake_action_tag);
        _qualityBox->setPosition(_originalPoint);
    }
}

void BaseCard::move(const Point& point, const std::function<void()>& callback)
{
    static const int zorder_top(1);
    static const int zorder_bottom(0);
    static const float duration(0.2f);
    static const int actionTag(818);
    if (getPosition() != point) {
        if (getActionByTag(actionTag)) {
            stopActionByTag(actionTag);
        }
        
        setLocalZOrder(zorder_top);
        auto action = Sequence::create(MoveTo::create(duration, point), CallFunc::create([=]() {
            setPosition(point);
            setLocalZOrder(zorder_bottom);
            if (callback) {
                callback();
            }
        }), nullptr);
        action->setTag(actionTag);
        runAction(action);
    } else if (callback) {
        callback();
    }
}

void BaseCard::updateProperty(const AbstractProperty* property)
{
    if (_icon && property) {
        auto cd(DataManager::getInstance()->getCardConfigData(property->getId()));
        if (cd) {
            _icon->setTexture(cd->getIcon());
        } else {
            _icon->initWithTexture(nullptr, Rect::ZERO);
        }
    }
    
    auto cp(dynamic_cast<const CardProperty*>(property));
    if (cp) {
        _costNode->setVisible(!cp->isHero());
        _cost->setString(StringUtils::format("%d", cp->getCost()));
    } else {
        _costNode->setVisible(false);
    }
    
    if (false) {
        _qualityBox->setTexture(CocosUtils::getResourcePath("ui_quality_box_blue.png"));
    }
}

void BaseCard::updateData(const AbstractData* data)
{
    AbstractCard::updateData(data);
}

void BaseCard::resetPositions(const Point& offset)
{
    for (auto child : _children) {
        child->setPosition(child->getPosition() + offset);
    }
}
