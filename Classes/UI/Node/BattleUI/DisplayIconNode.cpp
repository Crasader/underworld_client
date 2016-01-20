//
//  DisplayIconNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/8.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "DisplayIconNode.h"
#include "CocosGlobal.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;
using namespace UnderWorld::Core;

static const unsigned int maxCount = 3;
static const float cellOffsetX(5.0f);
static const float cellOffsetY(5.0f);

static Size iconSize(Size::ZERO);
static Size cellSize(Size::ZERO);
static Size tableViewMaxSize(Size::ZERO);

DisplayIconNode* DisplayIconNode::create(bool scrollToLeft)
{
    DisplayIconNode *ret = new (nothrow) DisplayIconNode();
    if (ret && ret->init(scrollToLeft))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

DisplayIconNode::DisplayIconNode()
:_observer(nullptr)
,_scrollToLeft(false)
,_basePosition(Point::ZERO)
,_currentCount(0)
,_touchInvalid(false)
,_tableView(nullptr)
{
    iconSize = UnitSimpleInfoNode::create(nullptr)->getContentSize() + Size(0, 18);
    cellSize = iconSize + Size(cellOffsetX, cellOffsetY * 2);
    tableViewMaxSize = Size(maxCount * cellSize.width, cellSize.height);
}

DisplayIconNode::~DisplayIconNode()
{
    removeAllChildren();
}

bool DisplayIconNode::init(bool scrollToLeft)
{
    if (Node::init()) {
        _scrollToLeft = scrollToLeft;
        
        _tableView = TableView::create(this, tableViewMaxSize);
        _tableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
        _tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        _tableView->setPosition(Point::ZERO);
        _tableView->setBounceable(false);
        addChild(_tableView);
        
        setContentSize(tableViewMaxSize);
        
        return true;
    }
    
    return false;
}

void DisplayIconNode::reload()
{
    // 1. save offset
    _contentOffset = _tableView->getContentOffset();
    
    // 2. reload data
    _tableView->reloadData();
    
    // 3. fit
    if (false == _tableView->isBounceable() && extension::ScrollView::Direction::HORIZONTAL == _tableView->getDirection()) {
        const size_t cnt = _camps.size();
        if (cnt >= maxCount) {
            // 3.1 reset offset
            _tableView->setContentOffset(_contentOffset);
            
            // 3.2 set view size if needed
            if (_tableView->getViewSize().width != tableViewMaxSize.width) {
                _tableView->setViewSize(tableViewMaxSize);
            }
            
            // 3.3 scroll if needed
            static const float duration = 0.5f;
            if (_scrollToLeft) {
                const float offsetX = (tableViewMaxSize - _tableView->getContentSize()).width;
                _tableView->setContentOffsetInDuration(Point(offsetX, 0), duration);
            } else {
                if (_contentOffset.x == 0)
                {
                    const float offsetX = _currentCount * cellSize.width - _tableView->getContentSize().width;
                    _tableView->setContentOffset(Point(offsetX, 0));
                }
                _tableView->setContentOffsetInDuration(Point::ZERO, duration);
            }
            
            // 3.4 reset position if needed
            if (_scrollToLeft && _position != _basePosition) {
                setPositionX(_basePosition.x);
            }
        } else {
            const Size size(cnt * cellSize.width, tableViewMaxSize.height);
            if (_tableView->getViewSize().width != size.width) {
                _tableView->setViewSize(size);
            }
            
            _tableView->setContentOffset(Point::ZERO);
            if (_scrollToLeft) {
                setPositionX(_basePosition.x + (maxCount - cnt) * cellSize.width);
            }
        }
    }
}

void DisplayIconNode::setPosition(const Point& position)
{
    Node::setPosition(position);
    
    if (_basePosition != position) {
        _basePosition = position;
        update();
    }
}

void DisplayIconNode::registerObserver(DisplayIconNodeObserver *observer)
{
    _observer = observer;
}

void DisplayIconNode::insert(const vector<pair<const Camp*, const UnitBase*>>& units)
{
    _currentCount = _camps.size();
    for (vector<pair<const Camp*, const UnitBase*>>::const_iterator iter = units.begin(); iter != units.end(); ++iter) {
        _camps.push_back(iter->first);
        _units.insert(make_pair(iter->first, iter->second));
    }
    reload();
}

void DisplayIconNode::update()
{
    _currentCount = _camps.size();
    reload();
}

#pragma mark - TableViewDataSource
Size DisplayIconNode::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
//    if (idx == _unitIds.size() - 1) {
//        return Size(cellSize.width + cellOffsetX, cellSize.height);
//    }
    
    return cellSize;
}

TableViewCell* DisplayIconNode::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TableViewCell* cell = table->dequeueCell();
    
    if (!cell) {
        cell = TableViewCell::create();
    }
    
    static const int tag = 100;
    const size_t cnt = _camps.size();
    const Camp* camp = _camps.at(_scrollToLeft ? idx : cnt - (idx + 1));
    const UnitBase* unit = _units.at(camp);
    UnitSimpleInfoNode* unitNode = dynamic_cast<UnitSimpleInfoNode*>(cell->getChildByTag(tag));
    if (unitNode) {
        unitNode->update(camp, unit);
    } else {
        unitNode = UnitSimpleInfoNode::create(camp, unit);
        unitNode->registerObserver(this);
        unitNode->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        unitNode->setPosition(Point(cellOffsetX, cellOffsetY));
        unitNode->setTag(tag);
        cell->addChild(unitNode);
    }
    
    return cell;
}

ssize_t DisplayIconNode::numberOfCellsInTableView(TableView *table)
{
    return _camps.size();
}

#pragma mark - UnitSimpleInfoNodeObserver
void DisplayIconNode::onUnitSimpleInfoNodeTouched(Ref* pSender, Widget::TouchEventType type, const UnitBase* unit)
{
    Widget* button = dynamic_cast<Widget*>(pSender);
    if (type == Widget::TouchEventType::BEGAN) {
        _touchInvalid = false;
    } else if (type == Widget::TouchEventType::MOVED) {
        if (!_touchInvalid && button->getTouchMovePosition().distance(button->getTouchBeganPosition()) > TOUCH_CANCEL_BY_MOVING_DISTANCE) {
            _touchInvalid = true;
        }
    } else if (type == Widget::TouchEventType::ENDED) {
        if (!_touchInvalid) {
            SoundManager::getInstance()->playButtonSound();
            if(_observer) {
                _observer->onDisplayIconNodeTouchedEnded(unit);
            }
        }
    }
}
