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

static const float tableNodeGapY(14);
static const vector<pair<string, string>> languages = {
    {"English", "English"},
    {"French", "Français"},
    {"German", "Deutsch"},
    {"Spanish", "Español"},
    {"Italian", "Italiano"},
    {"Dutch", "Nederlands"},
    {"Norwegian", "Norsk"},
    {"Portuguese", "Português"},
    {"Turkish", "Türkçe"},
    {"SimplifiedChinese", "简体中文"},
    {"Japanese", "日本語"},
    {"Korean", "한국어"},
    {"Russian", "Pусский"},
    {"TraditionalChinese", "繁體中文"},
    {"Arabic", "العربية"},
};

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
:_observer(nullptr)
,_table(nullptr)
,_nodeSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
,_selectedIdx(CC_INVALID_INDEX)
{
    _nodeSize = LanguageNode::create()->getContentSize();
}

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
        setContentSize(size);
        _tableMaxSize = size;
        _tableBasePosition = Point(0, size.height);
        
        createTable();
        
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
    const Size size(_tableMaxSize.width, _nodeSize.height + tableNodeGapY);
    auto cnt = getCellsCount();
    if (0 == idx || (cnt - 1) == idx) {
        return size + Size(0, tableNodeGapY / 2);
    }
    
    return size;
}

TableViewCell* LanguageLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    auto cnt = getCellsCount();
    static const float nodeIdx(0);
    auto node = dynamic_cast<LanguageNode*>(cell->getNode(nodeIdx));
    if (idx < cnt) {
        const auto& name(languages.at(idx).second);
        if (!node) {
            node = LanguageNode::create();
            node->registerObserver(this);
            cell->addChild(node);
            cell->setNode(node, nodeIdx);
        }
        
        node->update(name);
        node->setIdx(idx);
        node->tick(idx == _selectedIdx);
        
        // we must update the position when the table was reloaded
        const Point point(_tableMaxSize.width / 2, (_nodeSize.height + tableNodeGapY) / 2);
        node->setPosition(point + Point(0, (idx == cnt - 1) ? tableNodeGapY / 2 : 0));
    } else if (node) {
        node->removeFromParent();
        cell->resetNode(nodeIdx);
    }
    
    return cell;
}

ssize_t LanguageLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - LanguageNodeObserver
void LanguageLayer::onLanguageNodeSelected(ssize_t idx)
{
    auto prior(_selectedIdx);
    
    if (idx != _selectedIdx) {
        _selectedIdx = idx;
        
        if (CC_INVALID_INDEX != prior) {
            _table->updateCellAtIndex(prior);
        }
        
        if (CC_INVALID_INDEX != idx) {
            _table->updateCellAtIndex(idx);
        }
    }
}

#pragma mark - table
void LanguageLayer::createTable()
{
    auto tableView = TableView::create(this, _tableMaxSize);
    tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setBounceable(false);
    addChild(tableView);
    
    _table = tableView;
    refreshTable(false);
    tableView->setContentOffset(Point::ZERO);
}

void LanguageLayer::refreshTable(bool reload)
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

ssize_t LanguageLayer::getCellsCount() const
{
    return languages.size();
}
