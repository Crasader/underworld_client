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
#include "ChatUI.h"
#include "ChatManager.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "XTableViewCell.h"
#include "TabButton.h"

using namespace std;
using namespace ui;

static const size_t tableColumnCount(1);
static const Vec2 nodeOffsetOnTable(0, 14);
static const ChatType tableTypes[] = {
    ChatType::World,
    ChatType::Mail,
    ChatType::Notice
};
static const unsigned int tablesCount(sizeof(tableTypes) / sizeof(ChatType));

#pragma mark - TableNode
class ChatLayer::TableNode : public Node {
public:
    static TableNode* create(TableViewDataSource* source, const Size& size);
    virtual ~TableNode();
    void refresh(float height, bool reload);
    const TableView* getTable() const;
    
private:
    TableNode();
    bool init(TableViewDataSource* source, const Size& size);
    
private:
    Point _tableBasePosition;
    Size _tableMaxSize;
    TableView* _table;
};

ChatLayer::TableNode* ChatLayer::TableNode::create(TableViewDataSource* source, const Size& size)
{
    auto ret = new (nothrow) ChatLayer::TableNode();
    if (ret && ret->init(source, size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatLayer::TableNode::TableNode()
:_tableMaxSize(Size::ZERO)
,_table(nullptr) {}

ChatLayer::TableNode::~TableNode()
{
    removeAllChildren();
}

bool ChatLayer::TableNode::init(TableViewDataSource* source, const Size& size)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        auto background = CocosUtils::createBackground("GameImages/public/ui_background_1.png", size);
        background->setPosition(Point(size.width / 2, size.height / 2));
        addChild(background);
        
        static const Vec2 edge(2, 3);
        _tableMaxSize = Size(size.width - 2 * edge.x, size.height - 2 * edge.y);
        
        auto table = new (nothrow) TableView();
        table->initWithViewSize(size, nullptr);
        table->autorelease();
        table->setDataSource(source);
        table->setDirection(extension::ScrollView::Direction::VERTICAL);
        table->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        table->setBounceable(false);
        table->setContentOffset(Point::ZERO);
        background->addChild(table);
        
        _table = table;
        _tableBasePosition = edge + Point(0, _tableMaxSize.height);
        
        return true;
    }
    
    return false;
}

void ChatLayer::TableNode::refresh(float height, bool reload)
{
    if (_table) {
        auto size = Size(_tableMaxSize.width, MIN(height, _tableMaxSize.height));
        _table->setViewSize(size);
        _table->setPosition(_tableBasePosition - Point(0, size.height));
        
        if (reload) {
            const auto& offset(_table->getContentOffset());
            _table->reloadData();
            _table->setContentOffset(offset);
        }
    }
}

const TableView* ChatLayer::TableNode::getTable() const
{
    return _table;
}

#pragma mark - EditBoxNode
class ChatLayer::EditBoxNode : public Node {
public:
    static EditBoxNode* create(float width, EditBoxDelegate* delegate);
    virtual ~EditBoxNode();
    
private:
    EditBoxNode();
    bool init(float width, EditBoxDelegate* delegate);
    
private:
    ui::EditBox* _editBox;
    Button* _button;
};

ChatLayer::EditBoxNode* ChatLayer::EditBoxNode::create(float width, EditBoxDelegate* delegate)
{
    auto ret = new (nothrow) ChatLayer::EditBoxNode();
    if (ret && ret->init(width, delegate)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatLayer::EditBoxNode::EditBoxNode()
:_editBox(nullptr)
,_button(nullptr) {}

ChatLayer::EditBoxNode::~EditBoxNode()
{
    removeAllChildren();
}

bool ChatLayer::EditBoxNode::init(float width, EditBoxDelegate* delegate)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        static const string file(ChatUI::getResourcePath("button_liaotian.png"));
        _button = Button::create(file, file);
        addChild(_button);
        
        static const Vec2 edge(0, 0);
        const auto& bsize(_button->getContentSize());
        const Size size(width, bsize.height + edge.y * 2);
        setContentSize(size);
        
        static const Size ebBgSize(289, 38);
        static const float capInsetsOffset(6);
        Rect capInsets(capInsetsOffset, capInsetsOffset, ebBgSize.width - capInsetsOffset * 2, ebBgSize.height - capInsetsOffset * 2);
        auto s = ui::Scale9Sprite::create(ChatUI::getResourcePath("ui_kuang_7.png"), Rect(0, 0, ebBgSize.width, ebBgSize.height), capInsets);
        
        static const float space(3);
        static const float eEdgeY(3);
        const Size esize(width - (bsize.width + edge.x * 2 + space), bsize.height - eEdgeY * 2);
        _editBox = ui::EditBox::create(esize, s);
        _editBox->setDelegate(delegate);
        _editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        _editBox->setInputMode(ui::EditBox::InputMode::ANY);
        _editBox->setFontColor(Color4B::WHITE);
        _editBox->setPlaceholderFontSize(16);
        _editBox->setPlaceHolder("Please input...");
        addChild(_editBox);
        
        _editBox->setPosition(Point(edge.x + esize.width / 2, size.height / 2));
        _button->setPosition(Point(size.width - (edge.x + bsize.width / 2), size.height / 2));
        
        return true;
    }
    
    return false;
}

#pragma mark - ChatLayer
static const float viewWidth(385);
static const float subViewWidth(366);
static const int buttomZOrder(-1);
ChatLayer* ChatLayer::create()
{
    auto ret = new (nothrow) ChatLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatLayer::ChatLayer()
:_observer(nullptr)
,_folder(false)
,_isFolding(false)
,_background(nullptr)
,_button(nullptr)
,_buttonIcon(nullptr)
,_thisTableType(ChatType::None)
,_thisTableNode(nullptr)
,_editBoxNode(nullptr)
,_scrollBar(nullptr) {}

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
        const auto& winSize(Director::getInstance()->getWinSize());
        const Size size(viewWidth, winSize.height);
        setContentSize(size);
        
        // 1. button
        static const string file(ChatUI::getResourcePath("button_liaotian_1.png"));
        _button = Button::create(file, file);
        static const float bOffsetX(2);
        const auto& bsize(_button->getContentSize());
        _button->setPosition(Point(size.width + bsize.width / 2 - bOffsetX, size.height / 2));
        addChild(_button);
        
        _buttonIcon = Sprite::create(ChatUI::getResourcePath("icon_jiantou_1.png"));
        _buttonIcon->setPosition(Point(bOffsetX + _buttonIcon->getContentSize().width / 2, bsize.height / 2));
        _button->addChild(_buttonIcon);
        _button->setPressedActionEnabled(true);
        _button->addClickEventListener([this, size](Ref*) {
            if (!_isFolding) {
                _isFolding = true;
                _folder = !_folder;
                static const float duration(0.3f);
                runAction(Sequence::create(MoveTo::create(duration, _folder ? Point(-size.width, 0) : Point::ZERO), CallFunc::create([this]() {
                    _isFolding = false;
                    _buttonIcon->setScaleX(_folder ? 1 : -1);
                }), nullptr));
            }
        });
        
        // 2. background
        _background = CocosUtils::createBackground("GameImages/public/ui_background.png", size);
        _background->setPosition(Point(viewWidth / 2, winSize.height / 2));
        addChild(_background);
        
        static const float edgeTop(14);
        static const float edgeBottom(10);
        createTabButtons(Vec2(20, edgeTop));
        
        // tables
        static const float edgeLeft((viewWidth - subViewWidth) / 2);
        const float tbHeight(_tabButtons.begin()->second->getContentSize().height);
        const Point topLeft(edgeLeft, winSize.height - (edgeTop + tbHeight));
        for (int i = 0; i < tablesCount; ++i) {
            auto type = tableTypes[i];
            float height(topLeft.y - edgeBottom);
            if (ChatType::World == type) {
                _editBoxNode = EditBoxNode::create(subViewWidth, this);
                _background->addChild(_editBoxNode);
                
                const auto& size(_editBoxNode->getContentSize());
                _editBoxNode->setPosition(Point(viewWidth / 2, edgeBottom / 2 + size.height / 2));
                height -= size.height;
            }
            
            createTableNode(type, height, topLeft);
        }
        
        setTableType(ChatType::World);
        
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
    auto size = getCellSize(getTableType(table), idx);
    auto cnt = getCellsCount(table);
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
    
    auto type = getTableType(table);
    auto maxCnt(getDataSize(type));
    
    auto cnt = getCellsCount(table);
    for (int i = 0; i < tableColumnCount; ++i) {
        auto index = idx * tableColumnCount + i;
        Node* node(cell->getNode(i));
        if (ChatType::World == type) {
            node = dynamic_cast<ChatNode*>(node);
        } else if (ChatType::Mail == type) {
            node = dynamic_cast<NoticeNode*>(node);
        } else if (ChatType::Notice == type) {
            node = dynamic_cast<NoticeNode*>(node);
        }
        
        if (index < cnt) {
            if (!node) {
                node = createCellNode(type, index);
                cell->addChild(node);
                cell->setNode(node, i);
            }
            
            // we must update the position when the table was reloaded
            const auto& nodeSize(node->getContentSize());
            const Point point(nodeSize.width * (i + 0.5f) - nodeOffsetOnTable.x / 2, nodeSize.height * 0.5f);
            node->setPosition(point + Point(0, (idx == maxCnt - 1) ? nodeOffsetOnTable.y : 0));
        } else if (node) {
            node->removeFromParent();
            cell->resetNode(i);
        }
    }
    
    return cell;
}

ssize_t ChatLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount(table);
}

#pragma mark - EditBoxDelegate
void ChatLayer::editBoxTextChanged(ui::EditBox* editBox, const string& text)
{
    
}

void ChatLayer::editBoxReturn(ui::EditBox* editBox)
{
    
}

#pragma mark - ChatNodeObserver

#pragma mark - NoticeNodeObserver

#pragma mark - table
void ChatLayer::createTableNode(ChatType type, float height, const Point& topLeft)
{
    auto node = TableNode::create(this, Size(subViewWidth, height));
    node->setVisible(false);
    node->setTag(static_cast<int>(type));
    _background->addChild(node);
    
    // 1. insert table
    if (_tableNodes.find(type) != end(_tableNodes)) {
        assert(false);
    } else {
        _tableNodes.insert(make_pair(type, node));
    }
    
    auto table(node->getTable());
    if (_tableMappings.find(table) != end(_tableMappings)) {
        assert(false);
    } else {
        _tableMappings.insert(make_pair(table, node));
    }
    
    // 2. refresh table
    node->refresh(640, true);
    
    const auto& size(node->getContentSize());
    node->setPosition(topLeft + Point(size.width / 2, - size.height / 2));
}

ssize_t ChatLayer::getCellsCount(TableView* table) const
{
    auto type = getTableType(table);
    auto cnt = getDataSize(type);
    if (cnt > 0) {
        return (cnt - 1) / tableColumnCount + 1;
    }
    
    return 0;
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
void ChatLayer::createTabButtons(const Vec2& edge)
{
    const auto& size(_background->getContentSize());
    Size bsize(Size::ZERO);
    float space(0);
    for (int i = 0; i < tablesCount; ++i) {
        static const string normal(ChatUI::getResourcePath("button_fenye_1.png"));
        static const string select(ChatUI::getResourcePath("button_fenye.png"));
        auto type(tableTypes[i]);
        auto button = TabButton::create(getTableName(type), normal, select, [=](Ref*) {
            setTableType(type);
        });
        _background->addChild(button);
        _tabButtons.insert(make_pair(type, button));
        
        if (0 == i) {
            bsize = button->getContentSize();
            if (tablesCount > 1) {
                space = (size.width - (2 * edge.x + tablesCount * bsize.width)) / (tablesCount - 1);
            }
        }
        
        float x(0);
        if (tablesCount > 1) {
            x = edge.x + (i + 0.5) * bsize.width + i * space;
        } else {
            x = size.width / 2;
        }
        
        button->setPosition(Point(x, size.height - (edge.y + bsize.height / 2)));
    }
}

Size ChatLayer::getCellSize(ChatType type, size_t idx)
{
    // TODO: need to optimize
    auto node = createCellNode(type, idx);
    if (node) {
        auto size(node->getContentSize());
        return size;
    }
    
    return Size::ZERO;
}

Node* ChatLayer::createCellNode(ChatType type, size_t idx)
{
    Node* node(nullptr);
    const auto& data = ChatManager::getInstance()->getChatData(type);
    if (data.size() > idx) {
        static const float edgeX(3);
        static const float width(subViewWidth - edgeX * 2);
        if (ChatType::World == type) {
            node = ChatNode::create(width, data.at(idx));
            (dynamic_cast<ChatNode*>(node))->registerObserver(this);
        } else if (ChatType::Mail == type ||
                   ChatType::Notice == type) {
            node = NoticeNode::create(width, data.at(idx));
            (dynamic_cast<NoticeNode*>(node))->registerObserver(this);
        }
    }
    
    return node;
}

size_t ChatLayer::getDataSize(ChatType type) const
{
    return ChatManager::getInstance()->getChatData(type).size();
}

ChatType ChatLayer::getTableType(TableView* table) const
{
    if (table && _tableMappings.find(table) != end(_tableMappings)) {
        auto node(_tableMappings.at(table));
        if (node) {
            return static_cast<ChatType>(node->getTag());
        }
    }
    
    return ChatType::None;
}

void ChatLayer::setTableType(ChatType type)
{
    if (_thisTableType != type) {
        _thisTableType = type;
        
        for (auto iter = begin(_tableNodes); iter != end(_tableNodes); ++iter) {
            const bool isThisTable(iter->first == type);
            auto node = iter->second;
            node->setVisible(isThisTable);
            
            if (isThisTable) {
                _thisTableNode = node;
            }
        }
        
        for (auto iter = begin(_tabButtons); iter != end(_tabButtons); ++iter) {
            const bool isThisTable(iter->first == type);
            iter->second->setEnabled(!isThisTable);
        }
        
        _editBoxNode->setVisible(ChatType::World == type);
    }
}

string ChatLayer::getTableName(ChatType type) const
{
    switch (type) {
        case ChatType::World:
            return "公共";
        case ChatType::Mail:
            return "邮箱";
        case ChatType::Notice:
            return "系统";
        default:
            return "";
    }
}
