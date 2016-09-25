//
//  BillboardLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BillboardLayer.h"
#include "XTableViewCell.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "Board.h"
#include "BillboardManager.h"

using namespace std;

static const float tableNodeGapY(14);

BillboardLayer* BillboardLayer::create()
{
    BillboardLayer *ret = new (nothrow) BillboardLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BillboardLayer::BillboardLayer()
:_observer(nullptr)
,_table(nullptr)
,_nodeSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
,_selectedIdx(CC_INVALID_INDEX)
,_rank(nullptr)
,_trophy(nullptr) {}

BillboardLayer::~BillboardLayer()
{
    removeAllChildren();
}

void BillboardLayer::registerObserver(BillboardLayerObserver *observer)
{
    _observer = observer;
}

bool BillboardLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        auto board = Board::create(1);
        board->setTitle(LocalHelper::getString("ui_rank_title"));
        board->setExitCallback([this]() {
            removeFromParent();
        });
        board->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(board);
        
        const auto& size(board->getContentSize());
        static const float edgeTop(45);
        {
            static const float edgeLeft(20);
            auto hint = CocosUtils::createLabel(LocalHelper::getString("ui_rank_myRank"), BIG_FONT_SIZE);
            hint->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            hint->setPosition(edgeLeft, size.height - edgeTop);
            board->addChild(hint);
            
            auto label = CocosUtils::createLabel("", BIG_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(hint->getPosition() + Point(hint->getContentSize().width, 0));
            board->addChild(label);
            _rank = label;
        }
        
        {
            static const float edgeRight(130);
            auto label = CocosUtils::createLabel("", BIG_FONT_SIZE);
            label->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(size.width - edgeRight, size.height - edgeTop);
            board->addChild(label);
            _trophy = label;
            
            static const float lsOffset(0);
            const auto& lsize(label->getContentSize());
            auto sprite = Sprite::create(CocosUtils::getResourcePath("icon_jiangbei.png"));
            const auto& ssize(sprite->getContentSize());
            sprite->setPosition(label->getPosition() - Point(ssize.width / 2 + lsOffset, (lsize.height - ssize.height) / 2));
            board->addChild(sprite);
            
            static const float shOffset(0);
            auto hint = CocosUtils::createLabel(LocalHelper::getString("ui_rank_myTrophy"), BIG_FONT_SIZE);
            const auto& hsize(hint->getContentSize());
            hint->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
            hint->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
            hint->setPosition(sprite->getPosition() - Point(ssize.width / 2 + shOffset, (ssize.height - hsize.height) / 2));
            board->addChild(hint);
        }
        
        auto subNode(board->getSubNode(0));
        createTable(subNode);
        
        updateRank(30);
        updateTrophy(1000);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(BillboardLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(BillboardLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool BillboardLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void BillboardLayer::onTouchEnded(Touch *touch, Event *unused_event) {}

#pragma mark - TableViewDataSource
Size BillboardLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size(_tableMaxSize.width, _nodeSize.height + tableNodeGapY);
    auto cnt = getCellsCount();
    if (0 == idx || (cnt - 1) == idx) {
        return size + Size(0, tableNodeGapY / 2);
    }
    
    return size;
}

TableViewCell* BillboardLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    auto cnt = getCellsCount();
    static const float nodeIdx(0);
    auto node = dynamic_cast<BillboardCell*>(cell->getNode(nodeIdx));
    if (idx < cnt) {
        if (!node) {
            node = BillboardCell::create();
            node->registerObserver(this);
            cell->addChild(node);
            cell->setNode(node, nodeIdx);
        }
        
        if (idx < cnt) {
            auto data(BillboardManager::getInstance()->getBillboard().at(idx));
            node->update(data);
        }
        
        // we must update the position when the table was reloaded
        const Point point(_tableMaxSize.width / 2, (_nodeSize.height + tableNodeGapY) / 2);
        node->setPosition(point + Point(0, (idx == cnt - 1) ? tableNodeGapY / 2 : 0));
    } else if (node) {
        node->removeFromParent();
        cell->resetNode(nodeIdx);
    }
    
    return cell;
}

ssize_t BillboardLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - BillboardCellObserver
void BillboardLayer::onBillboardCellClicked(const BillboardData* data)
{
    
}

#pragma mark - table
void BillboardLayer::createTable(Node* parent)
{
    if (parent) {
        static const float edgeY(5.0f);
        const auto& size(parent->getContentSize());
        _nodeSize = BillboardCell::create()->getContentSize();
        _tableMaxSize = Size(size.width, size.height - edgeY * 2);
        _tableBasePosition = Point(0, size.height - edgeY);
        
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

void BillboardLayer::refreshTable(bool reload)
{
    if (_table) {
        auto totalHeight = _nodeSize.height * getCellsCount() + tableNodeGapY;
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

ssize_t BillboardLayer::getCellsCount() const
{
    return BillboardManager::getInstance()->getBillboard().size();
}

void BillboardLayer::updateRank(int rank)
{
    if (_rank) {
        _rank->setString(StringUtils::format("%d", rank));
    }
}

void BillboardLayer::updateTrophy(int trophy)
{
    if (_trophy) {
        _trophy->setString(StringUtils::format("%d", trophy));
    }
}
