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
#include "DevelopProperty.h"
#include "AbstractUpgradeProperty.h"

AbstractInfoLayer::AbstractInfoLayer()
:_board(nullptr)
,_icon(nullptr)
,_description(nullptr)
,_upgradeButton(nullptr)
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
    
    if (_board) {
        _board->setTitle(property ? LocalHelper::getString(property->getName()) : "");
    }
    
    if (_description) {
        _description->setString(property ? LocalHelper::getString(property->getDescription()) : "");
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
    
    if (data) {
        auto up(data->getUpgradeProperty());
        if (up) {
            const auto& pair(up->getResourceCost());
            if (pair.first != ResourceType::MAX) {
                _upgradeButton->setType(pair.first);
                _upgradeButton->setCount(pair.second);
                _upgradeButton->setEnabled(_icon ? _icon->canUpgrade() : false);
                static const bool enoughResource(true);
                _upgradeButton->setResourceEnough(enoughResource);
                
            } else {
                CC_ASSERT(false);
            }
        }
    }
    
    if (_upgradeButton) {
        _upgradeButton->setEnabled(nullptr != data);
    }
}
