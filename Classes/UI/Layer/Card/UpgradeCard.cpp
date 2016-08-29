//
//  UpgradeCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UpgradeCard.h"
#include "DevelopCard.h"
#include "ResourceButton.h"
#include "CocosUtils.h"
#include "DeckManager.h"
#include "CardSimpleData.h"

using namespace std;

UpgradeCard* UpgradeCard::create(int cardId)
{
    auto ret = new (nothrow) UpgradeCard();
    if (ret && ret->init(cardId)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UpgradeCard::UpgradeCard()
:_observer(nullptr)
,_icon(nullptr)
,_touchInvalid(false)
,_name(nullptr)
,_button(nullptr) {}

UpgradeCard::~UpgradeCard()
{
    removeAllChildren();
}

bool UpgradeCard::init(int cardId)
{
    if (ui::Widget::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setTouchEnabled(true);
        setSwallowTouches(false);
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, nullptr);
        
        auto card = DevelopCard::create(cardId);
        addChild(card);
        _icon = card;
        
        auto label = CocosUtils::createLabel("name", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        addChild(label);
        _name = label;
        
        auto button = ResourceButton::create(true, false, ResourceType::Gold, 3000, Color4B::BLACK, [this](Ref*) {
            if (_observer) {
                _observer->onUpgradeCardUpgrade(this);
            }
        });
        addChild(button);
        _button = button;
        
        const auto& isize(_icon->getContentSize());
        const auto& nsize(_name->getContentSize());
        const auto& bsize(_button->getContentSize());
        static const float spaceY(3);
        const float height(isize.height + nsize.height + bsize.height + spaceY * 2);
        const float width = MAX(MAX(isize.width, nsize.width), bsize.width);
        setContentSize(Size(width, height));
        
        _button->setPosition(width / 2, bsize.height / 2);
        _icon->setPosition(Point(width / 2, bsize.height + spaceY + isize.height / 2));
        _name->setPosition(width / 2, height - nsize.height / 2);
        
        {
            static const string file(CocosUtils::getResourcePath("button_lanse_3.png"));
            ui::Button* info = ui::Button::create(file, file);
            info->addClickEventListener([this](Ref*) {
                if (_observer) {
                    _observer->onUpgradeCardInfo(this);
                }
            });
            _icon->addChild(info);
            
            static const float edge(5);
            const auto& size(info->getContentSize());
            info->setPosition(Point(isize.width - (size.width / 2 + edge), isize.height - (size.height / 2 + edge)));
        }
        
        updateOthers(cardId);
        
        return true;
    }
    
    return false;
}

void UpgradeCard::registerObserver(UpgradeCardObserver *observer)
{
    _observer = observer;
}

void UpgradeCard::update(int cardId)
{
    if (_icon) {
        _icon->update(cardId);
    }
    
    if (getCardId() != cardId) {
        updateOthers(cardId);
    }
}

int UpgradeCard::getCardId() const
{
    if (_icon) {
        return _icon->getCardId();
    }
    
    return 0;
}

void UpgradeCard::updateOthers(int cardId)
{
    auto data = DeckManager::getInstance()->getCardData(cardId);
    if (_name) {
        _name->setString(data ? data->getName() : "");
    }
}
