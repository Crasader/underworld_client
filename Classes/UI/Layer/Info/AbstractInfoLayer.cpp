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
#include "CardPropertyNode.h"
#include "DataManager.h"
#include "AbstractData.h"
#include "AbstractProperty.h"
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
        updateProperty(DataManager::getInstance()->getProperty(cardId));
    }
    
    updateData(data);
}

void AbstractInfoLayer::updateProperty(const AbstractProperty* property)
{
    _property = property;
    
    if (_board) {
        _board->setTitle(property ? property->getName() : "");
    }
    
    if (_description) {
        _description->setString(property ? property->getDescription() : "");
    }
    
    for (int i = 0; i < _properties.size(); ++i) {
        auto property(_properties.at(i));
        if (property) {
            // TODO: property
        }
    }
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
