//
//  SpellInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SpellInfoLayer.h"
#include "JuniorCard.h"
#include "CardAttributeNode.h"
#include "MediumBoard.h"
#include "XButton.h"
#include "ResourceButton.h"
#include "PureScale9Sprite.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "DataManager.h"
#include "AbstractData.h"
#include "AbstractProperty.h"

using namespace std;

SpellInfoLayer* SpellInfoLayer::create(int cardId, const AbstractData* data)
{
    auto ret = new (nothrow) SpellInfoLayer();
    if (ret && ret->init(cardId, data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

SpellInfoLayer::SpellInfoLayer()
:_observer(nullptr) {}

SpellInfoLayer::~SpellInfoLayer()
{
    removeAllChildren();
}

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
    auto card = JuniorCard::create();
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
    
    // attributes
    {
        static const int rowCount(2);
        static const int columnCount(2);
        static const float spaceY(5);
        const float baseY(size.height - (secondaryEdge.y + descSize.height));
        float spaceX(0);
        Size attributeSize(Size::ZERO);
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
            
            auto attribute = CardAttributeNode::create(color);
            node->addChild(attribute);
            _attributes.push_back(attribute);
            
            // calculate space first
            if (0 == i) {
                attributeSize = attribute->getContentSize();
                spaceX = (descSize.width - attributeSize.width * columnCount) / (columnCount + 1);
                edgeY = (baseY - (attributeSize.height + spaceY) * rowCount + spaceY) / 2;
            }
            attribute->setPosition(size.width - (secondaryEdge.x + (columnCount - column) * (attributeSize.width + spaceX) - attributeSize.width / 2), baseY - edgeY - ((attributeSize.height + spaceY) * row + attributeSize.height / 2));
        }
    }
}

void SpellInfoLayer::initUI()
{
    const auto& winSize(Director::getInstance()->getWinSize());
    auto board = MediumBoard::create();
    board->setTitle("untitled");
    board->setExitCallback([this]() {
        if (_observer) {
            _observer->onSpellInfoLayerExit(this);
        }
    });
    board->setPosition(Point(winSize.width / 2, winSize.height / 2));
    addChild(board);
    _board = board;
    
    auto button = ResourceButton::create(true, false, ResourceType::Gold, 0, Color4B::BLACK);
    button->addClickEventListener([this](Ref* pSender) {
        auto button(dynamic_cast<ResourceButton*>(pSender));
        if (button) {
            if (button->isResourceEnough()) {
                if (_observer) {
                    _observer->onSpellInfoLayerUpgrade(this, _icon->getCardData());
                }
            } else {
                MessageBox("资源不足", nullptr);
            }
        }
    });
    board->addChildToMidBottom(button);
    _resourceButton = button;
    
    createNode(board->getSubNode());
}

void SpellInfoLayer::updateProperty(const DevelopProperty* property)
{
    AbstractInfoLayer::updateProperty(property);
}

void SpellInfoLayer::updateData(const AbstractData* data)
{
    AbstractInfoLayer::updateData(data);
}
