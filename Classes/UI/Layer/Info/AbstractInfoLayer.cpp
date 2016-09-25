//
//  AbstractInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/5.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AbstractInfoLayer.h"
#include "CocosGlobal.h"
#include "UniversalBoard.h"
#include "JuniorCard.h"
#include "ResourceButton.h"
#include "CardAttributeNode.h"
#include "LocalHelper.h"
#include "DataManager.h"
#include "AbstractData.h"
#include "CardProperty.h"
#include "AbstractUpgradeProperty.h"

AbstractInfoLayer::AbstractInfoLayer()
:_board(nullptr)
,_icon(nullptr)
,_description(nullptr)
,_resourceButton(nullptr)
,_data(nullptr)
,_property(nullptr) {}

AbstractInfoLayer::~AbstractInfoLayer()
{
    removeAllChildren();
}

bool AbstractInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void AbstractInfoLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

bool AbstractInfoLayer::init(int cardId, const AbstractData* data)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        initUI();
        update(cardId, data);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(AbstractInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(AbstractInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void AbstractInfoLayer::update(int cardId, const AbstractData* data)
{
    if (_icon && _icon->update(cardId, data)) {
        auto property(dynamic_cast<const DevelopProperty*>(DataManager::getInstance()->getProperty(cardId)));
        updateProperty(property);
    }
    
    updateData(data);
}

void AbstractInfoLayer::updateProperty(const DevelopProperty* property)
{
    _property = property;
    
    const bool canUpgrade(_icon && !dynamic_cast<const CardProperty*>(property));
    
    if (_icon) {
        _icon->setProgressBarVisible(canUpgrade);
    }
    
    if (_board) {
        _board->setTitle(property ? LocalHelper::getString(property->getName()) : "");
    }
    
    if (_description) {
        _description->setString(property ? LocalHelper::getString(property->getDescription()) : "");
    }
    
    if (_resourceButton) {
        _resourceButton->setVisible(canUpgrade);
    }
    
    do {
        const auto& cardAttributes(property->getAttributes());
        
        for (int i = 0; i < _attributes.size(); ++i) {
            auto node(_attributes.at(i));
            node->setVisible(i < cardAttributes.size());
        }
        
        int idx(0);
        const auto& orders(property->getOrderedAttributes());
        for (int i = 0; i < orders.size(); ++i) {
            const auto type(orders.at(i));
            auto iter(cardAttributes.find(type));
            if (idx < _attributes.size() && iter != end(cardAttributes)) {
                auto node(_attributes.at(idx));
                node->setAttribute(type, iter->second);
                
                if ((ATTR_GROUND_DAMAGE == type || ATTR_AIR_DAMAGE == type) && cardAttributes.find(ATTR_TARGET_TYPE) != end(cardAttributes)) {
                    auto targetType(static_cast<TargetType>(cardAttributes.at(ATTR_TARGET_TYPE)));
                    if (TargetType::BOTH != targetType || cardAttributes.find(ATTR_AIR_DAMAGE) == end(cardAttributes)) {
                        node->setName(LocalHelper::getString("ui_cardAttr_damage"));
                    }
                }
                ++ idx;
            }
        }
    } while (false);
}

void AbstractInfoLayer::updateData(const AbstractData* data)
{
    _data = data;
    
    do {
        CC_BREAK_IF(!_resourceButton || !_resourceButton->isVisible());
        _resourceButton->setEnabled(nullptr != data);
        CC_BREAK_IF(!data);
        auto up(data->getUpgradeProperty());
        CC_BREAK_IF(!up);
        const auto& pair(up->getResourceCost());
        auto type(pair.first);
        CC_BREAK_IF(type == ResourceType::MAX);
        _resourceButton->setType(type);
        _resourceButton->setCount(pair.second);
        _resourceButton->setEnabled(_icon ? _icon->canUpgrade() : false);
        static const bool enoughResource(true);
        _resourceButton->setResourceEnough(enoughResource);
    } while (false);
}
