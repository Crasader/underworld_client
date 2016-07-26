//
//  ChatLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatLayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "XTableViewCell.h"
#include "TabButton.h"

using namespace std;
using namespace ui;

static const unsigned int tableColumnCount(1);
static const Vec2 nodeOffsetOnTable(5, 14);
static const ChatTableType tableTypes[] = {
    ChatTableType::Talk,
    ChatTableType::Mail,
    ChatTableType::Notice
};
static const unsigned int tablesCount(sizeof(tableTypes) / sizeof(ChatTableType));

ChatLayer* ChatLayer::create()
{
    ChatLayer *ret = new (nothrow) ChatLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatLayer::ChatLayer()
:_observer(nullptr)
,_thisTableType(ChatTableType::None)
,_thisTable(nullptr)
,_nodeSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
,_editBoxBg(nullptr)
,_editBox(nullptr)
{
    auto size(ChatNode::create()->getContentSize());
    _nodeSize = size + Size(nodeOffsetOnTable.x, nodeOffsetOnTable.y);
    
    _tableMaxSize.width = _nodeSize.width * tableColumnCount + nodeOffsetOnTable.x;
    _tableMaxSize.height = _nodeSize.height;
}

ChatLayer::~ChatLayer()
{
    removeAllChildren();
}

void ChatLayer::registerObserver(ChatLayerObserver *observer)
{
    _observer = observer;
}

bool ChatLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        // tables
        for (int i = 0; i < tablesCount; ++i) {
            auto type = tableTypes[i];
            createTableView(type);
        }
        
        setTableType(ChatTableType::Talk);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(ChatLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(ChatLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool ChatLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void ChatLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDelegate
Size ChatLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount(table);
    if (idx == cnt - 1) {
        return size + Size(0, nodeOffsetOnTable.y);
    }
    
    return size;
}

TableViewCell* ChatLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    const ssize_t maxCnt = getCellsCount(table);
    const auto& messages = _messages.at(getTableType(table));
    const size_t cnt = messages.size();
    for (int i = 0; i < tableColumnCount; ++i) {
        const ssize_t index = idx * tableColumnCount + i;
        auto node = dynamic_cast<ChatNode*>(cell->getNode(i));
        if (index < cnt) {
            if (!node) {
                node = ChatNode::create();
                node->registerObserver(this);
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

#pragma mark - EditBoxDelegate
void ChatLayer::editBoxTextChanged(ui::EditBox* editBox, const string& text)
{
    
}

void ChatLayer::editBoxReturn(ui::EditBox* editBox)
{
    
}

ssize_t ChatLayer::numberOfCellsInTableView(TableView *table)
{
    return 1;
}

#pragma mark - ChatNodeObserver

#pragma mark - table
void ChatLayer::createTableView(ChatTableType type)
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

void ChatLayer::refreshTable(TableView* table, bool reload)
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

ssize_t ChatLayer::getCellsCount(TableView* table) const
{
    auto type = getTableType(table);
    if (_messages.find(type) != end(_messages)) {
        const auto& cards = _messages.at(type);
        const size_t cnt(cards.size());
        if (cnt > 0) {
            return (cnt - 1) / tableColumnCount + 1;
        }
    }
    
    return 0;
}

Size ChatLayer::getCellSize() const
{
    return Size(_tableMaxSize.width, _nodeSize.height);
}

Rect ChatLayer::getBoundingBox(Node* node) const
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
void ChatLayer::createTabButtons(const Point& position)
{
    
}

ChatNode* ChatLayer::createChatNode(const ChatData* data)
{
    auto node = ChatNode::create();
    return node;
}

void ChatLayer::createEditBox()
{
    _editBox = ui::EditBox::create(_editBoxBg->getContentSize(), nullptr);
    addChild(_editBox);
    _editBox->setPosition(_editBoxBg->getPosition());
    _editBox->setDelegate(this);
    // configuration
    _editBox->setMaxLength(120);
    _editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    _editBox->setInputMode(ui::EditBox::InputMode::ANY);
    
    _editBox->setFontColor(Color4B::WHITE);
    _editBox->setPlaceHolder("Please input...");
}

ChatTableType ChatLayer::getTableType(TableView* table) const
{
    if (table) {
        const size_t cnt = _tables.size();
        if (cnt < tablesCount) {
            return tableTypes[cnt];
        }
        
        return static_cast<ChatTableType>(table->getTag());
    }
    
    return ChatTableType::None;
}

void ChatLayer::setTableType(ChatTableType type)
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

string ChatLayer::getTableName(ChatTableType type) const
{
    switch (type) {
        case ChatTableType::Talk:
            return "公共";
        case ChatTableType::Mail:
            return "邮箱";
        case ChatTableType::Notice:
            return "系统";
        default:
            return "";
    }
}
