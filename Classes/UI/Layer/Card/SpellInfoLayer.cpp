//
//  SpellInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SpellInfoLayer.h"
#include "DevelopCard.h"
#include "CardPropertyNode.h"
#include "MediumBoard.h"
#include "UniversalButton.h"
#include "PureScale9Sprite.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "AbstractData.h"
#include "AbstractProperty.h"

using namespace std;

#pragma mark - SpellInfoLayer
SpellInfoLayer* SpellInfoLayer::create(const AbstractData* data)
{
    auto ret = new (nothrow) SpellInfoLayer();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SpellInfoLayer::SpellInfoLayer()
:_observer(nullptr)
,_board(nullptr)
,_icon(nullptr)
,_description(nullptr)
,_data(nullptr) {}

SpellInfoLayer::~SpellInfoLayer()
{
    removeAllChildren();
}

bool SpellInfoLayer::init(const AbstractData* data)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = MediumBoard::create();
        board->setTitle("untitled");
        board->setExitCallback([this]() {
            if (_observer) {
                _observer->onSpellInfoLayerExit(this);
            }
        });
        board->setButtonTitle(LocalHelper::getString("ui_cardInfo_upgrade"));
        board->setButtonCallback([this]() {
            if (_observer) {
                _observer->onSpellInfoLayerUpgrade(this, _data);
            }
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        _board = board;
        
        createNode(board->getSubNode());
        update(data);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(SpellInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(SpellInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool SpellInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void SpellInfoLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

void SpellInfoLayer::registerObserver(SpellInfoLayerObserver *observer)
{
    _observer = observer;
}

void SpellInfoLayer::createNode(Node* node)
{
    if (!node) {
        return;
    }
    
    const auto& size(node->getContentSize());
    
    static const float cardEdgeX(20);
    auto card = DevelopCard::create(nullptr);
    const auto& cardSize(card->getContentSize());
    card->setPosition(Point(cardEdgeX + cardSize.width / 2, size.height / 2));
    node->addChild(card);
    _icon = card;
    
    // description
    static const Vec2 secondaryEdge(10, 5);
    static const Size descSize(442, 90);
    {
        auto descBg = PureScale9Sprite::create(PureScale9Sprite::Type::White);
        descBg->setContentSize(descSize);
        descBg->setPosition(size.width - (secondaryEdge.x + descSize.width / 2), size.height - (secondaryEdge.y + descSize.height / 2));
        node->addChild(descBg);
        
        static const float edge(5);
        auto label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        label->setDimensions(descSize.width - edge * 2, descSize.height - edge * 2);
        label->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
        label->setTextColor(Color4B::BLACK);
        label->setPosition(descSize.width / 2, descSize.height / 2);
        descBg->addChild(label);
        _description = label;
    }
    
    // properties
    {
        static const int rowCount(2);
        static const int columnCount(2);
        static const float spaceY(5);
        const float baseY(size.height - (secondaryEdge.y + descSize.height));
        float spaceX(0);
        Size propertySize(Size::ZERO);
        float edgeY(0);
        for (int i = 0; i < 4; ++i) {
            const int row(i / columnCount);
            const int column(i % columnCount);
            Color4B color;
            if (row % 2 == 0) {
                color = PURE_GRAY;
            } else {
                color = PURE_WHITE;
            }
            
            auto property = CardPropertyNode::create(color);
            node->addChild(property);
            _properties.push_back(property);
            
            // calculate space first
            if (0 == i) {
                propertySize = property->getContentSize();
                spaceX = (descSize.width - propertySize.width * columnCount) / (columnCount + 1);
                edgeY = (baseY - (propertySize.height + spaceY) * rowCount + spaceY) / 2;
            }
            property->setPosition(size.width - (secondaryEdge.x + (columnCount - column) * (propertySize.width + spaceX) - propertySize.width / 2), baseY - edgeY - ((propertySize.height + spaceY) * row + propertySize.height / 2));
        }
    }
}

void SpellInfoLayer::update(const AbstractData* data)
{
    if (_data != data) {
        _data = data;
        
        if (_board) {
            _board->setTitle(data ? data->getName() : "");
        }
        
        if (_icon) {
            _icon->update(data);
        }
        
        if (_description) {
            _description->setString(data ? data->getProperty()->getDescription() : "");
        }
        
        for (int i = 0; i < _properties.size(); ++i) {
            auto property(_properties.at(i));
            if (property) {
                // TODO: property
            }
        }
    }
}
