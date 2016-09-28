//
//  TradeableCard.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TradeableCard.h"
#include "ResourceButton.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "CommodityData.h"
#include "ResourceManager.h"

using namespace std;

TradeableCard* TradeableCard::create()
{
    auto ret = new (nothrow) TradeableCard();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

TradeableCard::TradeableCard()
:_button(nullptr) {}

TradeableCard::~TradeableCard()
{
    removeAllChildren();
}

bool TradeableCard::init()
{
    if (BaseCard::init()) {
        auto button = ResourceButton::create(true, false, ResourceType::Gold, 0, Color4B::BLACK);
        addChild(button);
        _button = button;
        
        CocosUtils::fixWidgetTouchEvent(button, _touchInvalid, [this](Ref* pSender) {
            auto button(dynamic_cast<ResourceButton*>(pSender));
            if (button) {
                if (button->isResourceEnough()) {
                    if (_observer) {
                        _observer->onBaseCardClickedResourceButton(this);
                    }
                } else {
                    MessageBox("资源不足", nullptr);
                }
            }
        });
        
        const Size originalSize(getContentSize());
        const auto& bsize(_button->getContentSize());
        static const float buttonSpaceY(8);
        const float height(originalSize.height + bsize.height + buttonSpaceY);
        const float width = MAX(originalSize.width, bsize.width);
        setContentSize(Size(width, height));
        
        resetPositions(Point((width - originalSize.width) / 2, bsize.height + buttonSpaceY));
        _button->setPosition(Point(width / 2, bsize.height / 2));
        
        return true;
    }
    
    return false;
}

void TradeableCard::updateData(const AbstractData* data)
{
    BaseCard::updateData(data);
    
    do {
        CC_BREAK_IF(!_button);
        
        bool show(nullptr != data);
        _button->setVisible(show);
        CC_BREAK_IF(!show);
        
        auto ad(dynamic_cast<const CommodityData*>(data));
        CC_BREAK_IF(!ad);
        
        const auto& resources(ad->getPrice());
        show = (resources.size() > 0);
        _button->setVisible(show);
        CC_BREAK_IF(!show);
        
        auto iter(resources.begin());
        auto type(iter->first);
        show = (type != ResourceType::MAX);
        _button->setVisible(show);
        CC_BREAK_IF(!show);
        
        const auto count(iter->second);
        _button->setType(type);
        _button->setCount(count);
        
        static const bool enoughResource(ResourceManager::getInstance()->getResourceCount(type) >= count);
        _button->setResourceEnough(enoughResource);
        // TODO: check if the books is enough
        _button->setEnabled(enoughResource);
    } while (false);
}
