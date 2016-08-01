//
//  QuestLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestLayer.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "XTableViewCell.h"
#include "TabButton.h"
#include "DataManager.h"
#include "QuestData.h"
#include "QuestLocalData.h"

using namespace std;
using namespace ui;

static const unsigned int tableColumnCount(1);
static const Vec2 nodeOffsetOnTable(5, 14);
static const QuestType tableTypes[] = {
    QuestType::Main,
    QuestType::Branch,
    QuestType::Daily,
    QuestType::TimeLimited
};
static const unsigned int tablesCount = sizeof(tableTypes) / sizeof(QuestType);

static inline const Size& getWinSize() { return Director::getInstance()->getWinSize(); }

QuestLayer* QuestLayer::create(QuestType type)
{
    QuestLayer *ret = new (nothrow) QuestLayer();
    if (ret && ret->init(type))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

QuestLayer::QuestLayer()
:_observer(nullptr)
,_thisTableType(QuestType::None)
,_thisTable(nullptr)
,_nodeSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
{
    auto size = QuestNode::create(nullptr, 0)->getContentSize();
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

QuestLayer::~QuestLayer()
{
    removeAllChildren();
}

void QuestLayer::registerObserver(QuestLayerObserver *observer)
{
    _observer = observer;
}

bool QuestLayer::init(QuestType type)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        // tables
        for (int i = 0; i < tablesCount; ++i) {
            auto type = tableTypes[i];
            createTable(type);
        }
        
        // buttons
        CocosUtils::createGrayExitButton(this, [this]() { removeFromParent(); });
        {
            const auto& winSize = getWinSize();
            static const Vec2 edge(180, 50);
            createTabButtons(Point(edge.x, winSize.height - edge.y));
        }
        
        setTableType(QuestType::Main);
        
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
Size QuestLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount(table);
    if (idx == cnt - 1) {
        return size + Size(0, nodeOffsetOnTable.y);
    }
    
    return size;
}

TableViewCell* QuestLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    const ssize_t maxCnt = getCellsCount(table);
    const auto& quests = _candidates.at(getTableType(table));
    const size_t cnt = quests.size();
    for (int i = 0; i < tableColumnCount; ++i) {
        const ssize_t index = idx * tableColumnCount + i;
        auto node = dynamic_cast<QuestNode*>(cell->getNode(i));
        if (index < cnt) {
            if (!node) {
                node = QuestNode::create(nullptr, i);
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

ssize_t QuestLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount(table);
}

#pragma mark table
void QuestLayer::createTable(QuestType type)
{
    auto tableView = TableView::create(this, _tableMaxSize);
    tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setBounceable(false);
    tableView->setVisible(false);
    tableView->setTag(static_cast<int>(type));
    addChild(tableView);
    
    // 1. insert table
    if (_tables.find(type) == end(_tables)) {
        _tables.insert(make_pair(type, tableView));
    } else {
        assert(false);
    }
    
    // 2. refresh table
    refreshTable(tableView, false);
    tableView->setContentOffset(Point::ZERO);
}

void QuestLayer::refreshTable(TableView* table, bool reload)
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

ssize_t QuestLayer::getCellsCount(TableView* table) const
{
    auto type = getTableType(table);
    if (_candidates.find(type) != end(_candidates)) {
        const auto& quests = _candidates.at(type);
        const size_t cnt(quests.size());
        if (cnt > 0) {
            return (cnt - 1) / tableColumnCount + 1;
        }
    }
    
    return 0;
}

Size QuestLayer::getCellSize() const
{
    return Size(_tableMaxSize.width, _nodeSize.height);
}

Rect QuestLayer::getBoundingBox(Node* node) const
{
    if (node) {
        Rect rect = node->getBoundingBox();
        Point origin = rect.origin;
        rect.origin = convertToNodeSpace(node->getParent()->convertToWorldSpace(origin));
        return rect;
    }
    
    return Rect::ZERO;
}

#pragma mark buttons
void QuestLayer::createTabButtons(const Point& position)
{
    float width(0);
    float buttonHeight(0);
    static const Vec2 offset(10.0f, 0);
    for (int i = 0; i < tablesCount; ++i) {
        const auto type = tableTypes[i];
        const string title = getTableName(type);
        auto button = TabButton::create(title, [this, i](Ref*) {
            setTableType(tableTypes[i]);
        });
        addChild(button);
        
        const auto& size = button->getContentSize();
        button->setPosition(position + Point((size.width + offset.x) * i + size.width / 2, -(offset.y + size.height / 2)));
        
        if (_tabButtons.find(type) != end(_tabButtons)) {
            assert(false);
            _tabButtons.at(type) = button;
        } else {
            _tabButtons.insert(make_pair(type, button));
        }
        
        width += size.width + offset.x;
        buttonHeight = MAX(buttonHeight, size.height);
    }
    
    width -= offset.x;
    
    // add lines
    vector<Sprite*> lines;
    for (int i = 0; i < 2; ++i) {
        auto line = Sprite::create("GameImages/public/ui_line.png");
        line->setScaleX(width / line->getContentSize().width);
        addChild(line);
        
        lines.push_back(line);
    }
    
    const float x = position.x + width / 2;
    static const float offsetY(5.0f);
    lines.at(0)->setPosition(Point(x, position.y - buttonHeight - offsetY));
    lines.at(1)->setPosition(Point(x, position.y + offsetY));
}

#pragma mark - functions
void QuestLayer::reloadAllCandidates()
{
    _candidates.clear();
}

void QuestLayer::reloadCandidates(QuestType type)
{
    if (_candidates.find(type) != end(_candidates)) {
        _candidates.at(type).clear();
    }
}

void QuestLayer::insertCandidate(QuestType type, const string& name)
{
    if (_candidates.find(type) == end(_candidates)) {
        _candidates.insert(make_pair(type, vector<string>()));
    }
    
    auto& quests = _candidates.at(type);
    quests.push_back(name);
}

void QuestLayer::removeCandidate(QuestType type, const string& name)
{
    if (_candidates.find(type) != end(_candidates)) {
        auto& quests = _candidates.at(type);
        for (auto iter = begin(quests); iter != end(quests); ++iter) {
            if (name == *iter) {
                quests.erase(iter);
                break;
            }
        }
    }
}

QuestType QuestLayer::getTableType(TableView* table) const
{
    if (table) {
        const size_t cnt = _tables.size();
        if (cnt < tablesCount) {
            return tableTypes[cnt];
        }
        
        return static_cast<QuestType>(table->getTag());
    }
    
    return static_cast<QuestType>(-1);
}

void QuestLayer::setTableType(QuestType type)
{
    if (_thisTableType != type) {
        _thisTableType = type;
        
        for (auto iter = begin(_tables); iter != end(_tables); ++iter) {
            const bool isThisTable(iter->first == type);
            auto table = iter->second;
            table->setVisible(isThisTable);
            
            if (isThisTable) {
                _thisTable = table;
            }
        }
        
        for (auto iter = begin(_tabButtons); iter != end(_tabButtons); ++iter) {
            const bool isThisTable(iter->first == type);
            iter->second->setEnabled(!isThisTable);
        }
    }
}

string QuestLayer::getTableName(QuestType type) const
{
    switch (type) {
        case QuestType::Main:
            return "主线";
        case QuestType::Branch:
            return "支线";
        case QuestType::Daily:
            return "日常";
        case QuestType::TimeLimited:
            return "限时";
        default:
            return "";
    }
}
