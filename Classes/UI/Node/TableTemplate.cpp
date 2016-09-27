//
//  TableTemplate.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TableTemplate.h"
#include "XTableViewCell.h"

const float TableTemplate::DefaultGap = 14.0f;

TableTemplate::TableTemplate(Node* parent, const Point& position, bool horizontal, const Size& size, int elementCountPerCell, float gap, TableTemplateObserver* observer)
:_tag(Node::INVALID_TAG)
,_table(nullptr)
,_isHorizontal(horizontal)
,_nodeSize(Size::ZERO)
,_cellSize(Size::ZERO)
,_maxSize(size)
,_basePosition(position)
,_elementCountPerCell(MAX(elementCountPerCell, 1))
,_fixedGap(gap)
,_fixedGapSize(Size::ZERO)
,_unfixedGapSize(Size::ZERO)
,_observer(observer)
{
    do {
        CC_BREAK_IF(!parent || size.equals(Size::ZERO));
        CC_ASSERT(observer != nullptr);
        
        _nodeSize = _observer->onTableTemplateCreateNodeModel(this)->getContentSize();
        if (horizontal) {
            _cellSize = Size(_nodeSize.width + gap, size.height);
            _fixedGapSize.width = gap;
            _unfixedGapSize.height = (size.height - _nodeSize.height * elementCountPerCell) / (elementCountPerCell + 1);
        } else {
            _cellSize = Size(size.width, _nodeSize.height + gap);
            _fixedGapSize.height = gap;
            _unfixedGapSize.width = (size.width - _nodeSize.width * elementCountPerCell) / (elementCountPerCell + 1);
        }
        
        auto tableView = TableView::create(this, size);
        tableView->setDirection(horizontal ? extension::ScrollView::Direction::HORIZONTAL : extension::ScrollView::Direction::VERTICAL);
        tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        tableView->setBounceable(false);
        parent->addChild(tableView);
        _table = tableView;
        
        refreshTable(false);
    } while (false);
}

TableTemplate::~TableTemplate() {}

#pragma mark - public
void TableTemplate::setTag(int tag)
{
    _tag = tag;
}

void TableTemplate::setContentOffsetType(ContentOffsetType type)
{
    if (_table) {
        _table->setContentOffset(getContentOffset(type));
    }
}

void TableTemplate::refreshTable(bool reload)
{
    auto totalLength = _cellSize.height * getCellsCount() + _fixedGap;
    refreshTable(totalLength, reload, true);
}

void TableTemplate::refreshTable(float length, bool reload, bool keepContentOffset )
{
    do {
        CC_BREAK_IF(!_table);
        Size size(_maxSize);
        Point posOffset(Point::ZERO);
        if (!_isHorizontal) {
            posOffset.y = size.height = MIN(length, _maxSize.height);
        }
        _table->setViewSize(size);
        _table->setPosition(_basePosition - posOffset);
        
        CC_BREAK_IF(!reload);
        Vec2 offset(Vec2::ZERO);
        if (keepContentOffset) {
            offset = _table->getContentOffset();
        }
        _table->reloadData();
        _table->setContentOffset(offset);
    } while (false);
}

int TableTemplate::getTag() const
{
    return _tag;
}

TableView* TableTemplate::getTableView() const
{
    return _table;
}

#pragma mark - TableViewDataSource
Size TableTemplate::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    auto cnt = getCellsCount();
    if (0 == idx || (cnt - 1) == idx) {
        return _cellSize + _fixedGapSize / 2;
    }
    
    return _cellSize;
}

TableViewCell* TableTemplate::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    if (!cell) { cell = XTableViewCell::create(); }
    
    auto maxCnt(getCellsCount());
    auto cnt(_observer->numberOfNodesForTableTemplate(this));
    for (int i = 0; i < _elementCountPerCell; ++i) {
        auto index(idx * _elementCountPerCell + i);
        auto node(cell->getNode(i));
        if (index < cnt) {
            if (!node) {
                node = _observer->onTableTemplateCreateNodeModel(this);
                cell->addChild(node);
                cell->setNode(node, i);
            }
            
            _observer->onTableTemplateUpdateNode(this, index, node);
            
            Point point(_nodeSize.width / 2, _nodeSize.height / 2);
            if (_isHorizontal) {
                point.y = point.y + _nodeSize.height * i;
            } else {
                point.x = point.x + _nodeSize.width * i;
            }
            node->setPosition((point + _unfixedGapSize * (i + 1) + _fixedGapSize / 2) + ((idx == maxCnt - 1) ? _fixedGapSize / 2 : Size::ZERO));
        } else if (node) {
            node->removeFromParent();
            cell->resetNode(i);
        }
    }
    
    return cell;
}

ssize_t TableTemplate::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - private
Vec2 TableTemplate::getContentOffset(ContentOffsetType type) const
{
    Vec2 offset(Vec2::ZERO);
    
    if (_table) {
        switch (type) {
            case ContentOffsetType::BEGIN: {
                if (_isHorizontal) {
                    offset.x = -_table->getContentSize().width;
                } else {
                    offset.y = -_table->getContentSize().height;
                }
            }
                break;
            default:
                break;
        }
    }
    
    return offset;
}

ssize_t TableTemplate::getCellsCount() const
{
    auto cnt(_observer->numberOfNodesForTableTemplate(this));
    if (cnt > 0) {
        return (cnt - 1) / _elementCountPerCell + 1;
    }
    
    return 0;
}
