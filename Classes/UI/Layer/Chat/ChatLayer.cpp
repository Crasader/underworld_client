//
//  ChatLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatLayer.h"
#include "CocosUtils.h"
#include "ChatUI.h"
#include "ChatManager.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "LocalHelper.h"
#include "XTableViewCell.h"
#include "TabButton.h"

using namespace std;
using namespace ui;

static const size_t tableColumnCount(1);
static const Vec2 nodeGapOnTable(0, 10);
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
    TableView* getTable() const;
    
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
        
        auto background = CocosUtils::createBackground(CocosUtils::getResourcePath("ui_background_1.png"), size);
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

TableView* ChatLayer::TableNode::getTable() const
{
    return _table;
}

#pragma mark - EditBoxNode
class ChatLayer::EditBoxNode : public Node {
public:
    static EditBoxNode* create(float width, EditBoxDelegate* delegate, const function<void()>& callback);
    virtual ~EditBoxNode();
    void setFocus(bool focus);
    const char* getText() const;
    void setText(const char* msg);
    
private:
    EditBoxNode();
    bool init(float width, EditBoxDelegate* delegate, const function<void()>& callback);
    
private:
    ui::EditBox* _editBox;
    Button* _button;
};

ChatLayer::EditBoxNode* ChatLayer::EditBoxNode::create(float width, EditBoxDelegate* delegate, const function<void()>& callback)
{
    auto ret = new (nothrow) ChatLayer::EditBoxNode();
    if (ret && ret->init(width, delegate, callback)) {
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

bool ChatLayer::EditBoxNode::init(float width, EditBoxDelegate* delegate, const function<void()>& callback)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        static const string file(ChatUI::getResourcePath("button_liaotian.png"));
        _button = Button::create(file, file);
        _button->addClickEventListener([callback](Ref*) {
            if (callback) {
                callback();
            }
        });
        addChild(_button);
        
        const auto& bsize(_button->getContentSize());
        const Size size(width, bsize.height);
        setContentSize(size);
        
        static const Size ebBgSize(289, 38);
        static const float capInsetsOffset(6);
        Rect capInsets(capInsetsOffset, capInsetsOffset, ebBgSize.width - capInsetsOffset * 2, ebBgSize.height - capInsetsOffset * 2);
        auto s = ui::Scale9Sprite::create(ChatUI::getResourcePath("ui_kuang_7.png"), Rect(0, 0, ebBgSize.width, ebBgSize.height), capInsets);
        
        static const float space(3);
        static const float eEdgeY(3);
        const Size esize(width - (bsize.width + space), bsize.height - eEdgeY * 2);
        _editBox = ui::EditBox::create(esize, s);
        _editBox->setDelegate(delegate);
        _editBox->setMaxLength(esize.width);
        _editBox->setReturnType(ui::EditBox::KeyboardReturnType::SEND);
        _editBox->setInputMode(ui::EditBox::InputMode::ANY);
        _editBox->setFontColor(Color4B::BLACK);
        _editBox->setPlaceholderFontSize(16);
        _editBox->setPlaceHolder(LocalHelper::getString("ui_chat_placeholder").c_str());
        addChild(_editBox);
        
        _editBox->setPosition(Point(esize.width / 2, size.height / 2));
        _button->setPosition(Point(size.width - bsize.width / 2, size.height / 2));
        
        return true;
    }
    
    return false;
}

void ChatLayer::EditBoxNode::setFocus(bool focus)
{
    if (_editBox) {
        if (focus) {
            _editBox->touchDownAction(_editBox, Widget::TouchEventType::ENDED);
        } else {
            _editBox->onExit();
        }
    }
}

const char* ChatLayer::EditBoxNode::getText() const
{
    if (_editBox) {
        return _editBox->getText();
    }
    
    return nullptr;
}

void ChatLayer::EditBoxNode::setText(const char* msg)
{
    if (msg && _editBox) {
        _editBox->setText(msg);
    }
}

#pragma mark - ChatLayer
static const float viewWidth(385);
static const float subViewWidth(366);
static const int zorder_top(1);
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
,_background(nullptr)
,_buttonIcon(nullptr)
,_thisTableType(ChatType::None)
,_thisTableNode(nullptr)
,_editBoxNode(nullptr)
,_scrollBar(nullptr) {}

ChatLayer::~ChatLayer()
{
    removeAllChildren();
}

#pragma mark - public
void ChatLayer::registerObserver(ChatLayerObserver *observer)
{
    _observer = observer;
}

void ChatLayer::setButtonStatus(bool fold)
{
    if (_buttonIcon) {
        _buttonIcon->setScaleX(fold ? 1 : -1);
    }
}

void ChatLayer::setFocus(bool focus)
{
#if false
    if (_editBoxNode) {
        _editBoxNode->setFocus(focus && (ChatType::World == _thisTableType));
    }
#endif
}

#pragma mark - LayerColor
bool ChatLayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR)) {
        const auto& winSize(Director::getInstance()->getWinSize());
        const Size size(viewWidth, winSize.height);
        setContentSize(size);
        
        // 1. button
        static const string file(ChatUI::getResourcePath("button_liaotian_1.png"));
        auto button = Button::create(file, file);
        static const float bOffsetX(2);
        const auto& bsize(button->getContentSize());
        button->setPosition(Point(size.width + bsize.width / 2 - bOffsetX, size.height / 2));
        addChild(button);
        
        _buttonIcon = Sprite::create(ChatUI::getResourcePath("icon_jiantou_1.png"));
        _buttonIcon->setPosition(Point(bOffsetX + _buttonIcon->getContentSize().width / 2, bsize.height / 2));
        button->addChild(_buttonIcon);
        button->setPressedActionEnabled(true);
        button->addClickEventListener([this](Ref*) {
            if (_observer) {
                _observer->onChatLayerClickedButton();
            }
        });
        
        // 2. background
        _background = CocosUtils::createBackground(CocosUtils::getResourcePath("ui_background.png"), size);
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
                _editBoxNode = EditBoxNode::create(subViewWidth, this, CC_CALLBACK_0(ChatLayer::sendMessage, this));
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
    const auto& point = convertToNodeSpace(pTouch->getLocation());
    if (_background->getBoundingBox().containsPoint(point)) {
        return true;
    }
    
    return false;
}

void ChatLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDelegate
Size ChatLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    auto size = getCellSize(getTableType(table), idx);
    auto cnt = getCellsCount(table);
    if (0 == idx || (cnt - 1) == idx) {
        return size + Size(0, nodeGapOnTable.y / 2);
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
    auto maxCnt(getDataCount(type));
    auto cnt = getCellsCount(table);
    for (int i = 0; i < tableColumnCount; ++i) {
        auto index = idx * tableColumnCount + i;
        auto node(cell->getNode(i));
        if (index < maxCnt) {
            if (!node) {
                node = createCellNode(type, index);
                cell->addChild(node);
                cell->setNode(node, i);
            } else {
                updateCellNode(node, type, index);
            }
            
            // we must update the position when the table was reloaded
            const auto& nodeSize(node->getContentSize());
            const Point point(nodeSize.width * (i + 0.5f) + nodeGapOnTable.x / 2, nodeSize.height * 0.5f + nodeGapOnTable.y / 2);
            node->setPosition(point + Point(0, (idx == cnt - 1) ? nodeGapOnTable.y / 2 : 0));
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
    sendMessage();
}

#pragma mark - ChatNodeObserver
void ChatLayer::onChatNodeTouchedAvatar(const ChatData* data)
{
    if (data) {
        
    }
}

#pragma mark - NoticeNodeObserver
void ChatLayer::onNoticeNodeGet(const ChatData* data)
{
    if (data) {
        
    }
}

void ChatLayer::onNoticeNodeDelete(const ChatData* data)
{
    if (data) {
        
    }
}

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
    auto cnt = getDataCount(type);
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
            setFocus(true);
        });
        _background->addChild(button, zorder_top);
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
        return size + Size(nodeGapOnTable.x, nodeGapOnTable.y);
    }
    
    return Size::ZERO;
}

Node* ChatLayer::createCellNode(ChatType type, size_t idx)
{
    const auto& data = ChatManager::getInstance()->getChatData(type);
    if (data.size() > idx) {
        static const float edgeX(3);
        static const float width(subViewWidth - edgeX * 2);
        auto chatData(data.at(idx));
        if (ChatType::World == type) {
            auto cn = ChatNode::create(width, chatData);
            cn->registerObserver(this);
            return cn;
        } else if (ChatType::Mail == type ||
                   ChatType::Notice == type) {
            auto nn = NoticeNode::create(type, width, chatData);
            nn->registerObserver(this);
            return nn;
        }
    }
    
    return nullptr;
}

void ChatLayer::updateCellNode(Node* node, ChatType type, size_t idx)
{
    if (node) {
        const auto& data = ChatManager::getInstance()->getChatData(type);
        if (data.size() > idx) {
            auto chatData(data.at(idx));
            auto cn(dynamic_cast<ChatNode*>(node));
            if (cn) {
                cn->update(chatData);
            } else {
                auto nn(dynamic_cast<NoticeNode*>(node));
                if (nn) {
                    nn->update(chatData);
                }
            }
        }
    }
}

size_t ChatLayer::getDataCount(ChatType type) const
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
            return LocalHelper::getString("ui_chat_tab_world");
        case ChatType::Mail:
            return LocalHelper::getString("ui_chat_tab_mail");
        case ChatType::Notice:
            return LocalHelper::getString("ui_chat_tab_notice");
        default:
            return "";
    }
}

void ChatLayer::sendMessage()
{
    if (_editBoxNode) {
        static const auto type(ChatType::World);
        const char* text = _editBoxNode->getText();
        ChatManager::getInstance()->sendMessage(type, 1, text, [=](const char* msg) {
            _editBoxNode->setText("");
            
            if (_tableNodes.find(type) != end(_tableNodes)) {
                auto table = _tableNodes.at(type)->getTable();
                auto cnt(getCellsCount(table));
                if (cnt > 0) {
                    table->insertCellAtIndex(cnt - 1);
                } else {
                    CC_ASSERT(false);
                }
            }
        });
    }
}
