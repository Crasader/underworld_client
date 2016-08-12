//
//  BattleDeckCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleDeckCard.h"
#include "CocosUtils.h"
#include "BattleDeckUI.h"
#include "DataManager.h"
#include "CardSimpleData.h"
#include "UniversalButton.h"

using namespace std;

BattleDeckCard* BattleDeckCard::create(const CardSimpleData* data)
{
    auto ret = new (nothrow) BattleDeckCard();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleDeckCard::BattleDeckCard()
:_observer(nullptr)
,_icon(nullptr)
,_costNode(nullptr)
,_cost(nullptr)
,_hint(nullptr)
,_use(nullptr)
,_info(nullptr)
,_data(nullptr)
,_isHero(false)
,_inDeck(false)
,_touchInvalid(false) {}

BattleDeckCard::~BattleDeckCard()
{
    removeAllChildren();
}

bool BattleDeckCard::init(const CardSimpleData* data)
{
    if (Widget::init())
    {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(Size(Width, Height));
        
        _icon = Sprite::create("GameImages/icons/unit/icon_fashi.png");
        _icon->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
        _icon->setPosition(Width / 2, 0);
        addChild(_icon);
        
        static const float offsetY(4);
        _costNode = Sprite::create(CocosUtils::getResourcePath("ui_qiu.png"));
        _costNode->setPosition(Width / 2, Height - offsetY);
        addChild(_costNode);
        
        _cost = CocosUtils::createLabel("1", DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _cost->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        _cost->setAnchorPoint(Point::ANCHOR_MIDDLE);
        const auto& csize(_costNode->getContentSize());
        _cost->setPosition(csize.width / 2, csize.height / 2 + offsetY / 2);
        _costNode->addChild(_cost);
        
        static const Size size(110, 233);
        static const float capInsets(18.0f);
        static Rect capInset(capInsets, capInsets, size.width - capInsets * 2, size.height - capInsets * 2);
        _hint = ui::Scale9Sprite::create(BattleDeckUI::getResourcePath("ui_kuang_11.png"), Rect(Point::ZERO, size), capInset);
        addChild(_hint, -1);
        
        _use = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Green, "Use");
        _use->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onBattleDeckCardUse(this);
            }
        });
        addChild(_use);
        
        _info = UniversalButton::create(UniversalButton::BSize::Small, UniversalButton::BType::Blue, "Info");
        _info->setCallback([this](Ref*) {
            if (_observer) {
                _observer->onBattleDeckCardInfo(this);
            }
        });
        addChild(_info);
        
        setTouchEnabled(true);
        setSwallowTouches(false);
        addTouchEventListener([this](Ref *pSender, ui::Widget::TouchEventType type) {
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
                    _observer->onBattleDeckCardTouched(this);
                }
            }
        });
        
        showHint(false);
        
        return true;
    }
    
    return false;
}

void BattleDeckCard::registerObserver(BattleDeckCardObserver *observer)
{
    _observer = observer;
}

void BattleDeckCard::update(const CardSimpleData* data)
{
    if (_data != data) {
        _data = data;
    }
}

// TODO: remove this method
void BattleDeckCard::setIsHero(bool isHero)
{
    if (_isHero != isHero) {
        _isHero = isHero;
        static const string file("GameImages/icons/unit/");
        _icon->setTexture(file + (isHero ? "icon_shirenmo.png" : "icon_fashi.png"));
        _costNode->setVisible(!isHero);
    }
}

void BattleDeckCard::setInDeck(bool inDeck)
{
    if (_inDeck != inDeck) {
        _inDeck = inDeck;
    }
}

void BattleDeckCard::showHint(bool show)
{
    _hint->setVisible(show);
    _info->setVisible(show);
    
    
    if (show) {
        _use->setVisible(!_inDeck);
        
        static const float edgeX(5);
        static const float edgeTop(8);
        static const float edgeBottom(5);
        static const float space(5);
        const auto& iconSize(_icon->getContentSize());
        const auto& usize(_use->getContentSize());
        const auto& isize(_info->getContentSize());
        static const float pointX(Width / 2);
        const float baseY(_icon->getPositionY() - iconSize.height * _icon->getAnchorPoint().y);
        
        Size hsize(iconSize + Size(edgeX * 2, edgeTop + space + isize.height + edgeBottom));
        if (!_inDeck) {
            hsize.height = hsize.height + space + usize.height;
        }
        _hint->setContentSize(hsize);
        _hint->setPosition(pointX, Height + edgeTop - hsize.height / 2);
        
        if (_inDeck) {
            _info->setPosition(pointX, baseY - (space + isize.height / 2));
        } else {
            _use->setPosition(pointX, baseY - (space + usize.height / 2));
            _info->setPosition(pointX, baseY - (space * 2 + usize.height + isize.height / 2));
        }
    } else {
        _use->setVisible(false);
    }
}

bool BattleDeckCard::isShowHint() const
{
    return _use->isVisible();
}

const CardSimpleData* BattleDeckCard::getData() const
{
    return _data;
}
