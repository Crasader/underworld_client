//
//  LanguageLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "LanguageLayer.h"
#include "XTableViewCell.h"
#include "CocosGlobal.h"

using namespace std;

static const unsigned int tableColumnCount(10);
static const Vec2 nodeOffsetOnTable(5, 14);

LanguageLayer* LanguageLayer::create(const Size& size)
{
    LanguageLayer *ret = new (nothrow) LanguageLayer();
    if (ret && ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

LanguageLayer::LanguageLayer()
:_observer(nullptr) {}

LanguageLayer::~LanguageLayer()
{
    removeAllChildren();
}

void LanguageLayer::registerObserver(LanguageLayerObserver *observer)
{
    _observer = observer;
}

bool LanguageLayer::init(const Size& size)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        createTableView();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(LanguageLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(LanguageLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool LanguageLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return false;
}

void LanguageLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDataSource
Size LanguageLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount();
    if (idx == cnt - 1) {
        return size + Size(0, nodeOffsetOnTable.y);
    }
    
    return size;
}

TableViewCell* LanguageLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    const ssize_t maxCnt = getCellsCount();
    const size_t cnt = _languages.size();
    for (int i = 0; i < tableColumnCount; ++i) {
        const ssize_t index = idx * tableColumnCount + i;
        auto node = dynamic_cast<LanguageNode*>(cell->getNode(i));
        if (index < cnt) {
            const auto& name(_languages.at(index));
            if (!node) {
                node = LanguageNode::create(_languages.at(index));
                node->registerObserver(this);
                cell->addChild(node);
                cell->setNode(node, i);
            } else {
                node->update(name);
            }
            
            // we must update the position when the table was reloaded
            const Point point(_nodeSize.width * (i + 0.5f) - nodeOffsetOnTable.x / 2, node->getContentSize().height * 0.5f);
            node->setPosition(point + Point(0, (idx == maxCnt - 1) ? nodeOffsetOnTable.y : 0));
        } else if (node) {
            node->removeFromParent();
            cell->resetNode(i);
        }
    }
    
    return cell;
}

ssize_t LanguageLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - LanguageNodeObserver

#pragma mark - table
void LanguageLayer::createTableView()
{
    auto tableView = TableView::create(this, _tableMaxSize);
    tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setBounceable(false);
    tableView->setVisible(false);
    addChild(tableView);
    
    _table = tableView;
    refreshTable(false);
    tableView->setContentOffset(Point::ZERO);
}

void LanguageLayer::refreshTable(bool reload)
{
    if (_table) {
        auto totalHeight = _nodeSize.height * getCellsCount() + nodeOffsetOnTable.y;
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

ssize_t LanguageLayer::getCellsCount() const
{
    const size_t cnt(_languages.size());
    if (cnt > 0) {
        return (cnt - 1) / tableColumnCount + 1;
    }
    
    return 0;
}

Size LanguageLayer::getCellSize() const
{
    return Size(_tableMaxSize.width, _nodeSize.height);
}

Rect LanguageLayer::getBoundingBox(Node* node) const
{
    if (node) {
        Rect rect = node->getBoundingBox();
        Point origin = rect.origin;
        rect.origin = convertToNodeSpace(node->getParent()->convertToWorldSpace(origin));
        return rect;
    }
    
    return Rect::ZERO;
}
