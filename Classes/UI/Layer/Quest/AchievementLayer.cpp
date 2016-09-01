//
//  AchievementLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AchievementLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "XTableViewCell.h"
#include "DataManager.h"
#include "AchievementData.h"
#include "AchievementProperty.h"

using namespace std;
using namespace ui;

static const unsigned int tableColumnCount(1);
static const Vec2 nodeOffsetOnTable(5, 14);

static inline const Size& getWinSize() { return Director::getInstance()->getWinSize(); }

AchievementLayer* AchievementLayer::create()
{
    AchievementLayer *ret = new (nothrow) AchievementLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

AchievementLayer::AchievementLayer()
:_observer(nullptr)
,_table(nullptr)
,_thisTable(nullptr)
,_nodeSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
{
    auto size = AchievementNode::create(nullptr, 0)->getContentSize();
    _nodeSize = size + Size(nodeOffsetOnTable.x, nodeOffsetOnTable.y);
    
    // table parameters
    const auto& winSize = getWinSize();
    {
        static const Vec2 edge(50, 100);
        _tableMaxSize.width = _nodeSize.width * tableColumnCount + nodeOffsetOnTable.x;
        _tableMaxSize.height = winSize.height - edge.y;
        _tableBasePosition.x = winSize.width - edge.x - getCellSize().width;
        _tableBasePosition.y = winSize.height - edge.y;
    }
    
    reloadAllCandidates();
}

AchievementLayer::~AchievementLayer()
{
    removeAllChildren();
}

void AchievementLayer::registerObserver(AchievementLayerObserver *observer)
{
    _observer = observer;
}

bool AchievementLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        // tables
        createTable();
        
        // buttons
        CocosUtils::createGrayExitButton(this, [this]() { removeFromParent(); });
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [](Touch *pTouch, Event *pEvent) { return true; };
        eventListener->onTouchEnded = [](Touch *pTouch, Event *pEvent) {};
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

#pragma mark - TableViewDelegate
Size AchievementLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount(table);
    if (idx == cnt - 1) {
        return size + Size(0, nodeOffsetOnTable.y);
    }
    
    return size;
}

TableViewCell* AchievementLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    const ssize_t maxCnt = getCellsCount(table);
    const size_t cnt = _candidates.size();
    for (int i = 0; i < tableColumnCount; ++i) {
        const ssize_t index = idx * tableColumnCount + i;
        auto node = dynamic_cast<AchievementNode*>(cell->getNode(i));
        if (index < cnt) {
            if (!node) {
                node = AchievementNode::create(nullptr, i);
                cell->addChild(node);
                cell->setNode(node, i);
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

ssize_t AchievementLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount(table);
}

#pragma mark table
void AchievementLayer::createTable()
{
    auto tableView = TableView::create(this, _tableMaxSize);
    tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setBounceable(false);
    tableView->setVisible(false);
    addChild(tableView);
    
    // 1. insert table
    _table = tableView;
    
    // 2. refresh table
    refreshTable(tableView, false);
    tableView->setContentOffset(Point::ZERO);
}

void AchievementLayer::refreshTable(TableView* table, bool reload)
{
    if (table) {
        auto totalHeight = _nodeSize.height * getCellsCount(table) + nodeOffsetOnTable.y;
        auto size = Size(_tableMaxSize.width, MIN(totalHeight, _tableMaxSize.height));
        table->setViewSize(size);
        table->setPosition(_tableBasePosition - Point(0, size.height));
        
        if (reload) {
            const auto& offset = table->getContentOffset();
            table->reloadData();
            table->setContentOffset(offset);
        }
    }
}

ssize_t AchievementLayer::getCellsCount(TableView* table) const
{
    const size_t cnt(_candidates.size());
    if (cnt > 0) {
        return (cnt - 1) / tableColumnCount + 1;
    }
    
    return 0;
}

Size AchievementLayer::getCellSize() const
{
    return Size(_tableMaxSize.width, _nodeSize.height);
}

Rect AchievementLayer::getBoundingBox(Node* node) const
{
    if (node) {
        Rect rect = node->getBoundingBox();
        Point origin = rect.origin;
        rect.origin = convertToNodeSpace(node->getParent()->convertToWorldSpace(origin));
        return rect;
    }
    
    return Rect::ZERO;
}

#pragma mark - functions
void AchievementLayer::reloadAllCandidates()
{
    _candidates.clear();
}

void AchievementLayer::insertCandidate(const string& name)
{
    _candidates.push_back(name);
}

void AchievementLayer::removeCandidate(const string& name)
{
    for (auto iter = begin(_candidates); iter != end(_candidates); ++iter) {
        if (name == *iter) {
            _candidates.erase(iter);
            break;
        }
    }
}
