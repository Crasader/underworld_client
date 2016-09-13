//
//  GuildUISearchNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildUISearchNode.h"
#include "CocosUtils.h"
#include "XTableViewCell.h"
#include "XEditBox.h"
#include "XButton.h"

using namespace std;

static const float tableNodeGapY(14);

GuildUISearchNode* GuildUISearchNode::create(const Size& size)
{
    auto ret = new (nothrow) GuildUISearchNode();
    if (ret && ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildUISearchNode::GuildUISearchNode()
:_observer(nullptr)
,_table(nullptr)
,_nodeSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO) {}

GuildUISearchNode::~GuildUISearchNode()
{
    removeAllChildren();
}

void GuildUISearchNode::registerObserver(GuildUISearchNodeObserver *observer)
{
    _observer = observer;
}

bool GuildUISearchNode::init(const Size& size)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        static const float edgeY(15);
        auto editBox = XEditBox::create(Size(239, 50));
        const auto& ebsize(editBox->getContentSize());
        editBox->setDelegate(this);
        editBox->setFontSize(DEFAULT_FONT_SIZE);
        editBox->setPosition(Point(size.width / 2, size.height - (edgeY + ebsize.height / 2)));
        addChild(editBox);
        
        static const float spaceX(15);
        auto label = CocosUtils::createLabel("Search clans", BIG_FONT_SIZE);
        label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setTextColor(Color4B::BLACK);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        label->setPosition(Point(size.width / 2 - (ebsize.width / 2 + spaceX), editBox->getPositionY()));
        addChild(label);
        
        auto button = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        button->setTitleText("Search");
        button->setPosition(Point(size.width / 2 + (ebsize.width + button->getContentSize().width) / 2 + spaceX, editBox->getPositionY()));
        addChild(button);
        
        static const float edgeBottom(8);
        _nodeSize = GuildCell::create()->getContentSize();
        _tableMaxSize = Size(size.width, size.height - (ebsize.height + edgeY * 2 + edgeBottom));
        _tableBasePosition = Point(0, edgeBottom + _tableMaxSize.height);
        createTable();
        
        return true;
    }
    
    return false;
}

#pragma mark - TableViewDataSource
Size GuildUISearchNode::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size(_tableMaxSize.width, _nodeSize.height + tableNodeGapY);
    auto cnt = getCellsCount();
    if (0 == idx || (cnt - 1) == idx) {
        return size + Size(0, tableNodeGapY / 2);
    }
    
    return size;
}

TableViewCell* GuildUISearchNode::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    auto cnt = getCellsCount();
    static const float nodeIdx(0);
    auto node = dynamic_cast<GuildCell*>(cell->getNode(nodeIdx));
    if (idx < cnt) {
        if (!node) {
            node = GuildCell::create();
            node->registerObserver(this);
            cell->addChild(node);
            cell->setNode(node, nodeIdx);
        }
        
        if (idx < cnt) {
            node->update(_guilds.at(idx));
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

ssize_t GuildUISearchNode::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - EditBoxDelegate
void GuildUISearchNode::editBoxReturn(ui::EditBox* editBox)
{
    
}

#pragma mark - GuildCellObserver
void GuildUISearchNode::onGuildCellTouched(const GuildData* data)
{
    
}

#pragma mark - table
void GuildUISearchNode::createTable()
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

void GuildUISearchNode::refreshTable(bool reload)
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

ssize_t GuildUISearchNode::getCellsCount() const
{
    return _guilds.size();
}
