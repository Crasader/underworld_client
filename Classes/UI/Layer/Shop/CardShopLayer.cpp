//
//  CardShopLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardShopLayer.h"
#include "XTableViewCell.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "TradeableCard.h"
#include "Board.h"
#include "CommodityData.h"
#include "ShopManager.h"

using namespace std;

static const unsigned int column(6);
static const float tableNodeGapY(14);

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
,_table(nullptr)
,_nodeSize(Size::ZERO)
,_cellSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
,_tableNodeGapX(0) {}

CardShopLayer::~CardShopLayer()
{
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
        board->setTitle("Card Shop");
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        auto subNode(board->getSubNode(0));
        createTable(subNode);
        
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

#pragma mark - TableViewDataSource
Size CardShopLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    auto cnt = getCellsCount();
    if (0 == idx || (cnt - 1) == idx) {
        return _cellSize + Size(0, tableNodeGapY / 2);
    }
    
    return _cellSize;
}

TableViewCell* CardShopLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    auto maxCnt(getCellsCount());
    const auto& cards(ShopManager::getInstance()->getCardList());
    auto cnt(cards.size());
    for (int i = 0; i < column; ++i) {
        auto index(idx * column + i);
        auto node(dynamic_cast<TradeableCard*>(cell->getNode(i)));
        if (index < cnt) {
            if (!node) {
                node = TradeableCard::create();
                node->registerObserver(this);
                cell->addChild(node);
                cell->setNode(node, i);
            }
            
            auto data(cards.at(index));
            node->update(data->getId(), data);
            const Point point((_nodeSize.width + _tableNodeGapX) * (i + 1) - _nodeSize.width / 2, (_nodeSize.height + tableNodeGapY) / 2);
            node->setPosition(point + Point(0, (idx == maxCnt - 1) ? tableNodeGapY / 2 : 0));
        } else if (node) {
            node->removeFromParent();
            cell->resetNode(i);
        }
    }
    
    return cell;
}

ssize_t CardShopLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - BillboardCellObserver
void CardShopLayer::onBaseCardClickedResourceButton(BaseCard* pSender)
{
    if (pSender) {
        ShopManager::getInstance()->buyCard(pSender->getCardId(), [this](int cardId) {
            refreshTable(true);
        });
    }
}

#pragma mark - table
void CardShopLayer::createTable(Node* parent)
{
    if (parent) {
        static const float edgeY(5.0f);
        const auto& size(parent->getContentSize());
        _tableMaxSize = Size(size.width, size.height - edgeY * 2);
        _tableBasePosition = Point(0, size.height - edgeY);
        _nodeSize = TradeableCard::create()->getContentSize();
        _cellSize = Size(_tableMaxSize.width, _nodeSize.height + tableNodeGapY);
        _tableNodeGapX = (_tableMaxSize.width - _nodeSize.width * column) / (column + 1);
        
        auto tableView = TableView::create(this, _tableMaxSize);
        tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
        tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        tableView->setBounceable(false);
        parent->addChild(tableView);
        
        _table = tableView;
        refreshTable(false);
        tableView->setContentOffset(Point(0, -tableView->getContentSize().height));
    }
}

void CardShopLayer::refreshTable(bool reload)
{
    if (_table) {
        auto totalHeight = _cellSize.height * getCellsCount() + tableNodeGapY;
        auto size = Size(_tableMaxSize.width, MIN(totalHeight, _tableMaxSize.height));
        _table->setViewSize(size);
        _table->setPosition(_tableBasePosition - Point(0, size.height));
        
        if (reload) {
            const auto& offset = _table->getContentOffset();
            _table->reloadData();
            _table->setContentOffset(offset);
        }
    }
}

ssize_t CardShopLayer::getCellsCount() const
{
    auto cnt(ShopManager::getInstance()->getCardList().size());
    if (cnt > 0) {
        return (cnt - 1) / column + 1;
    }
    
    return 0;
}
