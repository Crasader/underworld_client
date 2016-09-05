//
//  SeniorCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SeniorCard.h"
#include "ResourceButton.h"
#include "CocosUtils.h"
#include "AbstractData.h"
#include "AbstractProperty.h"
#include "AbstractUpgradeProperty.h"

using namespace std;

SeniorCard* SeniorCard::create()
{
    auto ret = new (nothrow) SeniorCard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SeniorCard::SeniorCard()
:_name(nullptr)
,_button(nullptr) {}

SeniorCard::~SeniorCard()
{
    removeAllChildren();
}

bool SeniorCard::init()
{
    if (JuniorCard::init()) {
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
                        _observer->onBaseCardUpgrade(this);
                    }
                } else {
                    MessageBox("资源不足", nullptr);
                }
            }
        });
        addChild(button);
        _button = button;
        
        const Size originalSize(getContentSize());
        const auto& nsize(_name->getContentSize());
        const auto& bsize(_button->getContentSize());
        static const float nameSpaceY(15);
        static const float buttonSpaceY(8);
        const float height(originalSize.height + nsize.height + bsize.height + nameSpaceY + buttonSpaceY);
        const float width = MAX(MAX(originalSize.width, nsize.width), bsize.width);
        setContentSize(Size(width, height));
        
        resetPositions(Point((width - originalSize.width) / 2, bsize.height + buttonSpaceY));
        _button->setPosition(width / 2, bsize.height / 2);
        _name->setPosition(width / 2, height - nsize.height / 2);
        _infoButton->setVisible(true);
        
        return true;
    }
    
    return false;
}

void SeniorCard::updateProperty(const AbstractProperty* property)
{
    JuniorCard::updateProperty(property);
    if (_name) {
        _name->setString(property ? property->getName() : "");
    }
}

void SeniorCard::updateData(const AbstractData* data)
{
    JuniorCard::updateData(data);
    
    const bool show(nullptr != data);
    if (_button) {
        _button->setVisible(show);
    }
    
    if (show) {
        auto up(data->getUpgradeProperty());
        if (up) {
            const auto& pair(up->getResourceCost());
            if (pair.first != ResourceType::MAX) {
                _button->setType(pair.first);
                _button->setCount(pair.second);
                _button->setEnabled(canUpgrade());
                // TODO: if has enough resource
                static const bool enoughResource(true);
                _button->setResourceEnough(enoughResource);
            } else {
                CC_ASSERT(false);
            }
        }
    }
}
