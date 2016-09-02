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
#include "AbstractData.h"
#include "AbstractProperty.h"
#include "AbstractUpgradeProperty.h"

using namespace std;

UpgradeCard* UpgradeCard::create(const AbstractData* data)
{
    auto ret = new (nothrow) UpgradeCard();
    if (ret && ret->init(data)) {
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

bool UpgradeCard::init(const AbstractData* data)
{
    if (ui::Widget::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setTouchEnabled(true);
        setSwallowTouches(false);
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, nullptr);
        
        auto card = DevelopCard::create(nullptr);
        addChild(card);
        _icon = card;
        
        auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setTextColor(Color4B::BLACK);
        addChild(label);
        _name = label;
        
        auto button = ResourceButton::create(true, false, ResourceType::Gold, 0, Color4B::BLACK, [this](Ref* pSender) {
            auto button(dynamic_cast<ResourceButton*>(pSender));
            if (button) {
                if (button->isResourceEnough()) {
                    if (_observer) {
                        _observer->onUpgradeCardUpgrade(this);
                    }
                } else {
                    MessageBox("资源不足", nullptr);
                }
            }
        });
        addChild(button);
        _button = button;
        
        const auto& isize(_icon->getContentSize());
        const auto& nsize(_name->getContentSize());
        const auto& bsize(_button->getContentSize());
        static const float nameSpaceY(15);
        static const float buttonSpaceY(8);
        const float height(isize.height + nsize.height + bsize.height + nameSpaceY + buttonSpaceY);
        const float width = MAX(MAX(isize.width, nsize.width), bsize.width);
        setContentSize(Size(width, height));
        
        _button->setPosition(width / 2, bsize.height / 2);
        _icon->setPosition(Point(width / 2, bsize.height + buttonSpaceY + isize.height / 2));
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
        
        update(data);
        
        return true;
    }
    
    return false;
}

void UpgradeCard::registerObserver(UpgradeCardObserver *observer)
{
    _observer = observer;
}

void UpgradeCard::update(const AbstractData* data)
{
    if (_icon) {
        _icon->update(data);
    }
    
    const bool show(data ? data->isValid() : false);
    if (_name) {
        _name->setVisible(show);
    }
    
    if (_button) {
        _button->setVisible(show);
    }
    
    if (show) {
        if (_name) {
            _name->setString(data ? data->getProperty()->getName() : "");
        }
        
        if (data) {
            auto up(data->getUpgradeProperty());
            if (up) {
                const auto& pair(up->getResourceCost());
                if (pair.first != ResourceType::MAX) {
                    _button->setType(pair.first);
                    _button->setCount(pair.second);
                    _button->setEnabled(_icon ? _icon->canUpgrade() : false);
                    // TODO: if has enough resource
                    static const bool enoughResource(true);
                    _button->setResourceEnough(enoughResource);
                } else {
                    CC_ASSERT(false);
                }
            }
        }
    }
}

const AbstractData* UpgradeCard::getCardData() const
{
    if (_icon) {
        return _icon->getCardData();
    }
    
    return nullptr;
}
