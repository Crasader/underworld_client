//
//  BattleDeckLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleDeckLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "BattleDeckUnitInfoNode.h"
#include "DataManager.h"
#include "UserDefaultsDataManager.h"
#include "SoundManager.h"
#include "Utils.h"
#include "TechTree.h"
#include "BattleScene.h"

using namespace std;
using namespace ui;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const float unitNodeOffsetX(5.0f);
static const float unitNodeOffsetY(8.0f);
static const int topZOrder(1);
static const unsigned int cardDecksCount(8);
static const int cardTagOnDeck(100);

static const vector<UnitClass> tableUnitClass{kUnitClass_Hero, kUnitClass_Warrior};
static const map<UnitClass, set<string>> allCards{
    {
        kUnitClass_Hero,
        {
            "伊利丹",
            "泰兰德",
            "火法"
        }
    },
    {
        kUnitClass_Warrior,
        {
            "轻装武士",
            "窃贼",
            "骑士",
            "刺客",
            "疾风弓手",
            "火炮小队",
            "炼金师_毒气",
            "辉月侍僧",
            "流星投枪者",
            "天使",
            "巨鹰",
            "攻城弩"
        }
    }
};

BattleDeckLayer* BattleDeckLayer::create()
{
    BattleDeckLayer *ret = new (nothrow) BattleDeckLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleDeckLayer::BattleDeckLayer()
:_observer(nullptr)
,_tableViewPos(Point::ZERO)
,_infoNode(nullptr)
,_selectedCardsLabel(nullptr)
,_dragNode(nullptr)
{
    static const Size unitNodeSize = BattleDeckTestNode::create("", "", false, 0)->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY * 2 + 6.0f;
    _cellSize.width = unitNodeSize.width + unitNodeOffsetX;
    
    for (int i = 0; i < tableUnitClass.size(); ++i) {
        loadData(tableUnitClass.at(i));
    }
    
    _techTree = new (std::nothrow) TechTree();
    _techTree->init(DataManager::getInstance()->getTechTreeData(1));
}

BattleDeckLayer::~BattleDeckLayer()
{
    removeAllChildren();
    CC_SAFE_DELETE(_techTree);
}

void BattleDeckLayer::registerObserver(BattleDeckLayerObserver *observer)
{
    _observer = observer;
}

bool BattleDeckLayer::init()
{
    if (AbstractUILayer::init("Card Deck"))
    {
        Node* parent = _background;
        const Size& bgSize = parent->getContentSize();
        
        static const float ceilY(570.0f);
        static const float marginX(5.0f);
        static const float marginY(5.0f);
        
        // unit info node
        _infoNode = BattleDeckUnitInfoNode::create(nullptr, 0);
        parent->addChild(_infoNode);
        const Size& infoNodeSize = _infoNode->getContentSize();
        _infoNode->setPosition(Point(infoNodeSize.width / 2 + marginX, ceilY - (infoNodeSize.height / 2 + marginY)));
        
        // table views
        const float width = bgSize.width - (_infoNode->getPositionX() + infoNodeSize.width / 2);
        _tableViewPos.x = marginX + infoNodeSize.width + width / 2;
        _tableViewPos.y = ceilY - marginY;
        createTableViews(width);
        
        // card deck
        Sprite* title = CocosUtils::createTitle("", BIG_FONT_SIZE);
        _selectedCardsLabel = dynamic_cast<Label*>(title->getChildren().front());
        parent->addChild(title);
        
        const Size& titleSize = title->getContentSize();
        title->setPosition(Point(bgSize.width / 2, ceilY - (infoNodeSize.height + marginY * 2 + titleSize.height / 2)));
        
        // cards
        static const float offsetX(12.0f);
        static const float offsetY(8.0f);
        Point firstCardPos(Point::ZERO);
        for (int i = 0; i < cardDecksCount; ++i) {
            Sprite* sprite = Sprite::create("GameImages/test/ui_jiabing.png");
            const Size& size = sprite->getContentSize();
            if (firstCardPos.x == 0) {
                firstCardPos.x = marginX + offsetX + size.width / 2;
                firstCardPos.y = marginY + offsetY + size.height / 2;
            }
            sprite->setPosition(firstCardPos + Point((size.width + offsetX) * i, 0));
            parent->addChild(sprite);
            _cardDecks.push_back(sprite);
        }
        
        // ready button
        static const string file("GameImages/test/button_yellow_5.png");
        Button* button = Button::create(file, file);
        parent->addChild(button);
        
        const Size& buttonSize = button->getContentSize();
        button->setPosition(Point(bgSize.width - (buttonSize.width / 2 + marginX), buttonSize.height / 2 + marginY + offsetY));
        button->addClickEventListener([this](Ref*) {
            saveData();
            // TODO
            CocosUtils::replaceScene(BattleScene::create(1));
        });
        
        reloadCardDecks();
                
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(BattleDeckLayer::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(BattleDeckLayer::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(BattleDeckLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool BattleDeckLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void BattleDeckLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    const string& name = _selectedTableCard.second;
    const Point& point = _background->convertToNodeSpace(touch->getLocation());
    if (name.length() > 0) {
        TableView* table = _tableViewNodes.at(getUnitClass(name)).table;
        const cocos2d::Rect& rect = getRealBoundingBox(table);
        if (!rect.containsPoint(point)) {
            table->onTouchEnded(touch, unused_event);
        }
        onTableCardMoved(point);
    } else if (_selectedCard.length() > 0) {
        _dragNode->setPosition(point);
    }
}

void BattleDeckLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    const Point& point = _background->convertToNodeSpace(touch->getLocation());
    if (_selectedTableCard.second.length() > 0) {
        onTableCardEnded(point);
        
        // clear
        _selectedTableCard.first = CC_INVALID_INDEX;
        _selectedTableCard.second = "";
        removeDragNode();
    } else if (_selectedCard.length() > 0) {
        UnitClass uc;
        string exchangedCard;
        for (auto iter = begin(_tableViewNodes); iter != end(_tableViewNodes); ++iter) {
            TableView* table = iter->second.table;
            const cocos2d::Rect& rect = getRealBoundingBox(table);
            if (rect.containsPoint(point)) {
                // TODO
                uc = getUnitClass(table);
                exchangedCard = "";
            }
        }
        
        UnitClass pickedCardUnitClass = getUnitClass(_selectedCard);
        bool refreshUI(false);
        if (exchangedCard.length() > 0) {
            exchange(_selectedCard, uc, exchangedCard);
            refreshUI = true;
        } else if (CC_INVALID_INDEX == getIntersectedCardDeckIdx(_dragNode->getBoundingBox())) {
            insert(_selectedCard);
            refreshUI = true;
            // set a flag to avoid refresh the TableView
            uc = pickedCardUnitClass;
        }
        
        if (refreshUI) {
            // update UI
            configTable(pickedCardUnitClass, true);
            if (uc != pickedCardUnitClass) {
                configTable(uc, true);
            }
            reloadCardDecks();
        }
        
        // clear
        removeDragNode();
    }
}

#pragma mark - TableViewDelegate
void BattleDeckLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    
}

void BattleDeckLayer::tableCellHighlight(TableView* table, TableViewCell* cell)
{
    const UnitClass uc = getUnitClass(table);
    if (_candidateCards.find(uc) != _candidateCards.end()) {
        const vector<string>& vec = _candidateCards.at(uc);
        const ssize_t idx = cell->getIdx();
        if (vec.size() > idx) {
            _selectedTableCard.first = idx;
            _selectedTableCard.second = vec.at(idx);
        }
    }
}

#pragma mark - TableViewDataSource
Size BattleDeckLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const ssize_t cnt = getCellsCount(table);
    if (idx == cnt - 1) {
        return Size(_cellSize.width + unitNodeOffsetX, _cellSize.height);
    }
    
    return _cellSize;
}

TableViewCell* BattleDeckLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    TableViewCell *cell = table->dequeueCell();
    
    if (!cell) {
        cell = TableViewCell::create();
    }
    
    UnitClass uc = getUnitClass(table);
    const string& name = _candidateCards.at(uc).at(idx);
    const string& renderKey = getRenderKey(name);
    
    if (name.length() > 0) {
        static const int nodeTag(100);
        BattleDeckTestNode* unitNode = dynamic_cast<BattleDeckTestNode*>(cell->getChildByTag(nodeTag));
        if (unitNode) {
            unitNode->reuse(name, renderKey);
            unitNode->setSelected(name == _touchedCard.second);
        } else {
            unitNode = BattleDeckTestNode::create(name, renderKey, (kUnitClass_Hero == uc), _techTree->findUnitTypeByName(name)->getRarity());
            unitNode->registerObserver(this);
            unitNode->reuse(name, renderKey);
            const Size& size = unitNode->getContentSize();
            unitNode->setPosition(Point(size.width / 2, size.height / 2) + Point(unitNodeOffsetX, unitNodeOffsetY));
            unitNode->setTag(nodeTag);
            cell->addChild(unitNode);
        }
    }
    
    return cell;
}

ssize_t BattleDeckLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount(table);
}

#pragma mark - BattleDeckUnitNodeObserver
void BattleDeckLayer::onBattleDeckUnitNodeTouchedBegan(BattleDeckUnitNode* node)
{
    
}

void BattleDeckLayer::onBattleDeckUnitNodeTouchedEnded(BattleDeckUnitNode* node, bool isValid)
{
    
}

void BattleDeckLayer::onBattleDeckUnitNodeTouchedCanceled(BattleDeckUnitNode* node)
{
    
}

#pragma mark - BattleDeckTestNodeObserver
void BattleDeckLayer::onBattleDeckTestNodeTouchedBegan(BattleDeckTestNode* node)
{
    _selectedCard = node->getUnitName();
    createDragNode(_selectedCard);
    _dragNode->setPosition(_background->convertToNodeSpace(node->getParent()->convertToWorldSpace(node->getPosition())));
    _dragNode->setVisible(true);
}

void BattleDeckLayer::onBattleDeckTestNodeTouchedEnded(BattleDeckTestNode* node, bool isValid)
{
    removeDragNode();
    
    const string& lastCard = _touchedCard.second;
    const string& name = node->getUnitName();
    if (lastCard != name) {
        TableView* lastTable = _touchedCard.first;
        TableView* table(nullptr);
        if (_pickedCards.find(name) == end(_pickedCards)) {
            for (auto iter = begin(allCards); iter != end(allCards); ++iter) {
                const auto uc = (*iter).first;
                const auto cards = (*iter).second;
                if (cards.find(name) != end(cards)) {
                    table = _tableViewNodes.at(uc).table;
                    break;
                }
            }
        }
        
        _touchedCard.first = table;
        _touchedCard.second = name;
        _infoNode->update(name, _techTree);
        
        if (lastTable == table) {
            if (table) {
                reloadTable(table);
            } else {
                selectCardOnDecks(name);
            }
        } else {
            if (lastTable) {
                reloadTable(lastTable);
            } else {
                selectCardOnDecks(name);
            }
            
            if (table) {
                reloadTable(table);
            } else {
                selectCardOnDecks(name);
            }
        }
    }
}

void BattleDeckLayer::onBattleDeckTestNodeTouchedCanceled(BattleDeckTestNode* node)
{
    
}

#pragma mark - protected
void BattleDeckLayer::createTableViews(float width)
{
    static float offsetY(5.0f);
    vector<Node*> created;
    for (int i = 0; i < tableUnitClass.size(); ++i) {
        Node* node = createTableView(tableUnitClass[i], width);
        if (node) {
            const size_t cnt = created.size();
            Point pos(_tableViewPos);
            if (cnt > 0) {
                Node* n = created.back();
                pos = n->getPosition() - Point(0, n->getContentSize().height + offsetY);
            }
            node->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            node->setPosition(pos);
            _background->addChild(node);
            created.push_back(node);
        }
    }
    
    // reload data after all TableViews have been created
    for (auto iter = begin(_tableViewNodes); iter != end(_tableViewNodes); ++iter)
    {
        reloadTable(iter->second.table);
    }
}

Node* BattleDeckLayer::createTableView(UnitClass uc, float width)
{
    if (true) {
        TableViewNode tableViewNode;
        Node* parent = Node::create();
        
        // title
        Sprite* title = CocosUtils::createTitle("", BIG_FONT_SIZE);
        tableViewNode.titleLabel = dynamic_cast<Label*>(title->getChildren().front());
        parent->addChild(title);
        
        // table view
        static const float offsetX(5.0f);
        static const float offsetY(5.0f);
        
        static const string leftFile("GameImages/public/button_sanjiao_3.png");
        Button* leftButton = Button::create(leftFile, leftFile);
        parent->addChild(leftButton);
        tableViewNode.leftButton = leftButton;
        
        static const string rightFile("GameImages/public/button_sanjiao_2.png");
        Button* rightButton = Button::create(rightFile, rightFile);
        parent->addChild(rightButton);
        tableViewNode.rightButton = rightButton;
        
        const float leftButtonWidth = leftButton->getContentSize().width;
        const float rightButtonWidth = rightButton->getContentSize().width;
        tableViewNode.maxWidth = width - (leftButtonWidth + rightButtonWidth + offsetX * 2);
        
        TableView* tableView = TableView::create(this, _cellSize);
        tableView->setDelegate(this);
        tableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
        tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        tableView->setBounceable(false);
        tableView->setTag(uc);
        parent->addChild(tableView);
        tableViewNode.table = tableView;
        
        leftButton->addClickEventListener([](Ref*) {
            
        });
        rightButton->addClickEventListener([](Ref*) {
            
        });
        
        // set positions
        const Size& titleSize = title->getContentSize();
        const Size size(width, titleSize.height + offsetY + _cellSize.height);
        parent->setContentSize(size);
        
        title->setPosition(Point(size.width / 2, size.height - titleSize.height / 2));
        leftButton->setPosition(Point(leftButtonWidth / 2, _cellSize.height / 2));
        rightButton->setPosition(Point(size.width - rightButtonWidth / 2, leftButton->getPositionY()));
        tableView->setPosition(Point(leftButtonWidth + offsetX, 0));
        _tableViewNodes.insert(make_pair(uc, tableViewNode));
        
        // reset
        configTable(uc, false);
        
        return parent;
    }
    
    return nullptr;
}

UnitClass BattleDeckLayer::getUnitClass(TableView* table) const
{
    const size_t createdCount = _tableViewNodes.size();
    if (createdCount < tableUnitClass.size()) {
        return tableUnitClass[createdCount];
    }
    
    return static_cast<UnitClass>(table->getTag());
}

ssize_t BattleDeckLayer::getCellsCount(TableView* table) const
{
    return getCellsCount(getUnitClass(table));
}

ssize_t BattleDeckLayer::getCellsCount(UnitClass uc) const
{
    if (_candidateCards.find(uc) != _candidateCards.end()) {
        return _candidateCards.at(uc).size();
    }
    
    return 0;
}

cocos2d::Rect BattleDeckLayer::getRealBoundingBox(Node* node) const
{
    cocos2d::Rect rect = node->getBoundingBox();
    Point origin = rect.origin;
    rect.origin = _background->convertToNodeSpace(node->getParent()->convertToWorldSpace(origin));
    return rect;
}

void BattleDeckLayer::reloadTable(TableView* table)
{
    if (table) {
        const Point& offset = table->getContentOffset();
        table->reloadData();
        table->setContentOffset(offset);
    }
}

void BattleDeckLayer::configTable(UnitClass uc, bool reload)
{
    if (_tableViewNodes.find(uc) != _tableViewNodes.end()) {
        TableViewNode& node = _tableViewNodes.at(uc);
        const ssize_t cnt = getCellsCount(uc);
        const float totalWidth = _cellSize.width * cnt + unitNodeOffsetX;
        const float maxWidth = node.maxWidth;
        const Size tableSize(MIN(maxWidth, totalWidth), _cellSize.height);
        const bool canScroll(maxWidth < totalWidth);
        node.leftButton->setVisible(canScroll);
        node.rightButton->setVisible(canScroll);
        
        TableView* table = node.table;
        table->setViewSize(tableSize);
        if (reload) {
            reloadTable(table);
        }
        
        string message;
        if (kUnitClass_Hero == uc) {
            message = "Heroes";
        } else if (kUnitClass_Warrior == uc) {
            message = "Warriors";
        }
        node.titleLabel->setString(message + StringUtils::format(":%ld", cnt));
    }
}

void BattleDeckLayer::createDragNode(const string& name)
{
    if (!_dragNode && name.length() > 0) {
        _dragNode = BattleDeckTestNode::create(name, getRenderKey(name), getUnitClass(name), _techTree->findUnitTypeByName(name)->getRarity());
        _dragNode->setVisible(false);
        _background->addChild(_dragNode, topZOrder);
    }
}

void BattleDeckLayer::removeDragNode()
{
    if (_dragNode) {
        _dragNode->removeFromParent();
        _dragNode = nullptr;
        
        _selectedCard = "";
    }
}

void BattleDeckLayer::reloadCardDecks()
{
    const set<string>& cards = getPickedCards();
    const size_t cnt = _cardDecks.size();
    // remove all
    for (int i = 0; i < cnt; ++i) {
        Sprite* deck = _cardDecks.at(i);
        auto node = deck->getChildByTag(cardTagOnDeck);
        if (node) {
            deck->removeChild(node, true);
        }
    }
    // add
    int i = 0;
    for (auto iter = begin(cards); iter != end(cards); ++iter, ++i) {
        if (cnt > i) {
            Sprite* deck = _cardDecks.at(i);
            const string& name = *iter;
            const string& renderKey = getRenderKey(name);
            auto node = BattleDeckTestNode::create(name, renderKey, getUnitClass(name), _techTree->findUnitTypeByName(name)->getRarity());
            node->registerObserver(this);
            node->setTag(cardTagOnDeck);
            deck->addChild(node);
            const Size& size = deck->getContentSize();
            node->setPosition(Size(size.width / 2, size.height / 2));
        }
    }
    
    _selectedCardsLabel->setString(StringUtils::format("Troops:%d/%d", i, cardDecksCount));
}

void BattleDeckLayer::selectCardOnDecks(const string& name)
{
    for (int i = 0; i < _cardDecks.size(); ++i) {
        Sprite* deck = _cardDecks.at(i);
        BattleDeckTestNode* node = dynamic_cast<BattleDeckTestNode*>(deck->getChildByTag(cardTagOnDeck));
        if (node) {
            const string& unitName = node->getUnitName();
            node->setSelected(unitName == name);
        }
    }
}

void BattleDeckLayer::onTableCardMoved(const Point& pos)
{
    const string& name = _selectedTableCard.second;
    if (!_dragNode) {
        createDragNode(name);
    }
    
    TableView* table = _tableViewNodes.at(getUnitClass(name)).table;
    const cocos2d::Rect& rect = getRealBoundingBox(table);
    _dragNode->setVisible(!(rect.containsPoint(pos)));
    _dragNode->setPosition(pos);
}

void BattleDeckLayer::onTableCardEnded(const Point& pos)
{
    if (_dragNode) {
        int idx = getIntersectedCardDeckIdx(_dragNode->getBoundingBox());
        if (INVALID_VALUE == idx) {
            cardBackToTable();
        } else if (getPickedCards().size() < cardDecksCount) {
            const string& name = _selectedTableCard.second;
            UnitClass uc = getUnitClass(name);
            extract(uc, name);
            
            // update UI
            configTable(uc, true);
            reloadCardDecks();
            if (name == _touchedCard.second) {
                selectCardOnDecks(name);
            }
        }
    }
}

void BattleDeckLayer::cardBackToTable()
{
    removeDragNode();
}

int BattleDeckLayer::getIntersectedCardDeckIdx(const cocos2d::Rect& rect) const
{
    float intersectedArea(0);
    int idx(INVALID_VALUE);
    for (int i = 0; i < _cardDecks.size(); ++i) {
        const cocos2d::Rect& bd = _cardDecks.at(i)->getBoundingBox();
        if (bd.intersectsRect(rect)) {
            const Size& size = bd.unionWithRect(rect).size;
            float area = size.width * size.height;
            if (0 == intersectedArea || intersectedArea > area) {
                intersectedArea = area;
                idx = i;
            }
        }
    }
    
    return idx;
}

#pragma mark - Logic
template<typename T> static void eraseFromVector(vector<T>& vec, const T& element)
{
    for (auto iter = begin(vec); iter != end(vec); ++iter) {
        if (element == *iter) {
            vec.erase(iter);
            break;
        }
    }
}

static bool sortByUnitClass(const string& first, const string& second)
{
    UnitClass firstUC(UNIT_CLASS_COUNT);
    UnitClass secondUC(UNIT_CLASS_COUNT);
    for (auto iter = begin(allCards); iter != end(allCards); ++iter) {
        UnitClass uc = iter->first;
        const set<string>& set = iter->second;
        if (set.find(first) != end(set)) {
            firstUC = uc;
        }
        if (set.find(second) != end(set)) {
            secondUC = uc;
        }
        
        if (firstUC != UNIT_CLASS_COUNT && secondUC != UNIT_CLASS_COUNT) {
            break;
        }
    }
    
    if (firstUC != secondUC) {
        return (firstUC > secondUC);
    } else {
        return (first > second);
    }
    
    return false;
}

static bool sortByName(const string& first, const string& second)
{
    return (first > second);
}

string BattleDeckLayer::getRenderKey(const string& name) const
{
    if (_techTree) {
        const UnitType* unitType = _techTree->findUnitTypeByName(name);
        if (unitType) {
            return unitType->getRenderKey();
        }
    }
    
    assert(false);
    return "";
}

static const vector<string> emptyVector;
const vector<string>& BattleDeckLayer::getCandidateCards(UnitClass uc) const
{
    if (_candidateCards.find(uc) != _candidateCards.end()) {
        return _candidateCards.at(uc);
    }
    
    return emptyVector;
}

const set<string>& BattleDeckLayer::getPickedCards() const
{
    return _pickedCards;
}

UnitClass BattleDeckLayer::getUnitClass(const string& name) const
{
    for (auto iter = begin(allCards); iter != end(allCards); ++iter) {
        UnitClass uc = iter->first;
        const set<string>& set = iter->second;
        if (set.find(name) != end(set)) {
            return uc;
        }
    }
    
    return UNIT_CLASS_COUNT;
}

void BattleDeckLayer::loadData(UnitClass uc)
{
    if (0 == _pickedCards.size()) {
        UserDefaultsDataManager::getInstance()->getSelectedCards(_pickedCards);
    }
    
    if (allCards.find(uc) != allCards.end()) {
        const set<string>& total = allCards.at(uc);
        if (_candidateCards.find(uc) == _candidateCards.end()) {
            _candidateCards.insert(make_pair(uc, vector<string>()));
        }
        
        vector<string>& vec = _candidateCards.at(uc);
        vec.clear();
        
        set_difference(begin(total), end(total), begin(_pickedCards), end(_pickedCards), inserter(vec, begin(vec)));
        sort(begin(vec), end(vec), sortByName);
    }
}

void BattleDeckLayer::saveData()
{
    UserDefaultsDataManager::getInstance()->setSelectedCards(_pickedCards);
}

void BattleDeckLayer::extract(UnitClass uc, const string& name)
{
    // 1. erase
    if (_candidateCards.find(uc) != _candidateCards.end()) {
        vector<string>& vec = _candidateCards.at(uc);
        eraseFromVector(vec, name);
    }
    
    // 2. insert
    _pickedCards.insert(name);
//    sort(begin(_pickedCards), end(_pickedCards), sortByUnitClass);
}

void BattleDeckLayer::insert(const string& name)
{
    // 1. erase
    _pickedCards.erase(name);
    
    // 2. insert
    UnitClass uc = getUnitClass(name);
    
    if (UNIT_CLASS_COUNT != uc) {
        if (_candidateCards.find(uc) == _candidateCards.end()) {
            _candidateCards.insert(make_pair(uc, vector<string>()));
        }
        
        vector<string>& vec = _candidateCards.at(uc);
        vec.push_back(name);
        sort(begin(vec), end(vec), sortByName);
    }
}

void BattleDeckLayer::exchange(const string& picked, UnitClass uc, const string& candidate)
{
    extract(uc, candidate);
    insert(picked);
}
