//
//  DisplayIconNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/8.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "DisplayIconNode.h"
#include "CocosGlobal.h"
#include "DataManager.h"
#include "SoldierLocalData.h"
#include "URConfigData.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

static const Size iconSize(66, 66);
static const unsigned int maxCount = 3;
static const float cellOffsetX(5.0f);
static const float cellOffsetY(5.0f);

static const Size cellSize = iconSize + Size(cellOffsetX, cellOffsetY * 2);
static const Size tableViewMaxSize(maxCount * cellSize.width, cellSize.height);

DisplayIconNode* DisplayIconNode::create(bool scrollToLeft, const Point& position)
{
    DisplayIconNode *ret = new (nothrow) DisplayIconNode();
    if (ret && ret->init(scrollToLeft, position))
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
,_currentCount(0)
,_touchInvalid(false)
,_tableView(nullptr)
{
    
}

DisplayIconNode::~DisplayIconNode()
{
    removeAllChildren();
}

bool DisplayIconNode::init(bool scrollToLeft, const Point& position)
{
    if (Node::init()) {
        _scrollToLeft = scrollToLeft;
        _basePosition = position;
        
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
        const size_t cnt = _unitIds.size();
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
                if (_contentOffset == Point::ZERO)
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

void DisplayIconNode::registerObserver(DisplayIconNodeObserver *observer)
{
    _observer = observer;
}

void DisplayIconNode::addIcon(int unitId)
{
    _currentCount = _unitIds.size();
    _unitIds.push_back(unitId);
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
    Button* iconButton = dynamic_cast<Button*>(cell->getChildByTag(tag));
    const size_t cnt = _unitIds.size();
    const int id = _unitIds.at(_scrollToLeft ? idx : cnt - (idx + 1));
    const string& name = DataManager::getInstance()->getSoldierData(id)->getName();
    const string& file = DataManager::getInstance()->getURConfigData(name)->getSmallIcon();
    if (iconButton) {
        iconButton->loadTextures(file, file);
    } else {
        iconButton = Button::create(file, file);
        iconButton->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        iconButton->setPosition(Point(cellOffsetX, cellOffsetY));
        iconButton->setTag(tag);
        iconButton->setPressedActionEnabled(false);
        iconButton->setSwallowTouches(false);
        cell->addChild(iconButton);
    }
    
    if (iconButton) {
        iconButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
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
                        _observer->onDisplayIconNodeTouchedEnded( _unitIds.at(_scrollToLeft ? idx : cnt - (idx + 1)));
                    }
                }
            }
        });
    }
    
    return cell;
}

ssize_t DisplayIconNode::numberOfCellsInTableView(TableView *table)
{
    return _unitIds.size();
}