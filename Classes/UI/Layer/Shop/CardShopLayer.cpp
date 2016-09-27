//
//  CardShopLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardShopLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "TradeableCard.h"
#include "Board.h"
#include "CommodityData.h"
#include "ShopManager.h"
#include "DataManager.h"
#include "AbstractProperty.h"
#include "UniversalUIHelper.h"

using namespace std;

static const unsigned int column(6);

CardShopLayer* CardShopLayer::create()
{
    auto ret = new (nothrow) CardShopLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardShopLayer::CardShopLayer()
:_observer(nullptr)
,_tableTemplate(nullptr) {}

CardShopLayer::~CardShopLayer()
{
    CC_SAFE_DELETE(_tableTemplate);
    removeAllChildren();
}

void CardShopLayer::registerObserver(CardShopLayerObserver *observer)
{
    _observer = observer;
}

bool CardShopLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(1);
        board->setTitle(LocalHelper::getString("ui_cardShop_title"));
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        do {
            auto subNode(board->getSubNode(0));
            CC_BREAK_IF(!subNode);
            static const float edgeY(5.0f);
            const auto& subNodeSize(subNode->getContentSize());
            Size size(subNodeSize.width, subNodeSize.height - edgeY * 2);
            Point position(0, subNodeSize.height - edgeY);
            _tableTemplate = new (nothrow) TableTemplate(subNode, position, false, size, column, TableTemplate::DefaultGap, this);
            _tableTemplate->setContentOffsetType(TableTemplate::ContentOffsetType::BEGIN);
        } while (false);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(CardShopLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(CardShopLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool CardShopLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void CardShopLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - TableTemplateObserver
Node* CardShopLayer::onTableTemplateCreateNodeModel(TableTemplate* tt)
{
    auto node(TradeableCard::create());
    node->registerObserver(this);
    return node;
}

void CardShopLayer::onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node)
{
    do {
        CC_BREAK_IF(idx < 0 || !node);
        const auto& cards(ShopManager::getInstance()->getCardList());
        CC_BREAK_IF(idx >= cards.size());
        auto card(dynamic_cast<TradeableCard*>(node));
        CC_BREAK_IF(!card);
        auto data(cards.at(idx));
        card->update(data->getId(), data);
    } while (false);
}

ssize_t CardShopLayer::numberOfNodesForTableTemplate(const TableTemplate* tt)
{
    return ShopManager::getInstance()->getCardList().size();
}

#pragma mark - BillboardCellObserver
void CardShopLayer::onBaseCardClicked(BaseCard* pSender)
{
    if (pSender) {
        UniversalUIHelper::getInstance()->showCardInfoLayer(this, pSender->getCardId(), nullptr);
    }
}

void CardShopLayer::onBaseCardClickedResourceButton(BaseCard* pSender)
{
    if (pSender) {
        ShopManager::getInstance()->buyCard(pSender->getCardId(), [this](int cardId) {
            auto property(DataManager::getInstance()->getProperty(cardId));
            if (property && !property->getName().empty()) {
                UniversalUIHelper::getInstance()->showMessage(LocalHelper::getString("ui_cardShop_hasPurchased") + LocalHelper::getString(property->getName()));
            }
            
            if (_tableTemplate) {
                _tableTemplate->refreshTable(true);
            }
        });
    }
}
