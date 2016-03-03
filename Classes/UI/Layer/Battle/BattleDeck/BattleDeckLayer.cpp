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
#include "BattleDeckCell.h"
#include "BattleScene.h"

using namespace std;
using namespace ui;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const float unitNodeOffsetX(5.0f);
static const float unitNodeOffsetY(14.0f);
static const int topZOrder(1);
static const int cardTagOnDeck(100);
static const unsigned int columnCount(6);

static const set<string> allCards {
    // heroes
    "伊利丹",
    "泰兰德",
    "火法",
    // warriors
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
,_isExtracting(false)
,_techTree(nullptr)
,_infoNode(nullptr)
,_selectedCardsLabel(nullptr)
,_dragNode(nullptr)
{
    static const Size unitNodeSize = CardNode::create()->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY;
    _cellSize.width = unitNodeSize.width + unitNodeOffsetX;
    
    loadData();
    
    _techTree = new (nothrow) TechTree();
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
        Node* tableNode = createTableView(Size(width, infoNodeSize.height));
        tableNode->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
        tableNode->setPosition(marginX + infoNodeSize.width + width / 2, ceilY - marginY);
        parent->addChild(tableNode);
        
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
        for (int i = 0; i < CANDIDATE_CARD_DECKS_COUNT; ++i) {
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
        
        _closeButton->addClickEventListener([this](Ref*) {
            saveData();
            removeFromParent();
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
    const Point& point = _background->convertToNodeSpace(touch->getLocation());
    if (_selectedCard.length() > 0) {
        if (_isExtracting) {
            TableView* table = _tableViewNode.table;
            const cocos2d::Rect& rect = getRealBoundingBox(table);
            if (!rect.containsPoint(point)) {
                table->onTouchEnded(touch, unused_event);
            }
            
            if (!_dragNode) {
                createDragNode(_selectedCard);
            }
            
            _dragNode->setVisible(!(rect.containsPoint(point)));
        }
        
        if (_dragNode) {
            _dragNode->setPosition(point);
        }
    }
}

void BattleDeckLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    const Point& point = _background->convertToNodeSpace(touch->getLocation());
    if (_selectedCard.length() > 0 && _dragNode) {
        const int idx = getIntersectedCardDeckIdx(_dragNode->getBoundingBox());
        bool refreshUI(false);
        if (_isExtracting) {
            if (CC_INVALID_INDEX == idx) {
                cardBackToTable();
            } else {
                const size_t cnt = getPickedCards().size();
                if (cnt < CANDIDATE_CARD_DECKS_COUNT) {
                    extract(_selectedCard);
                    refreshUI = true;
                } else {
                    if (_cardDecks.size() > idx) {
                        CardNode* node = dynamic_cast<CardNode*>(_cardDecks.at(idx)->getChildByTag(cardTagOnDeck));
                        if (node) {
                            exchange(node->getCardName(), _selectedCard);
                            refreshUI = true;
                        }
                    }
                }
            }
            
            // clear
            _isExtracting = false;
        } else {
            if (CC_INVALID_INDEX == idx) {
                insert(_selectedCard);
                refreshUI = true;
            } else {
                string exchangedCard;
                TableView* table = _tableViewNode.table;
                const cocos2d::Rect& rect = getRealBoundingBox(table);
                if (rect.containsPoint(point)) {
                    // TODO:
                    exchangedCard = "";
                }
                
                if (exchangedCard.length() > 0) {
                    exchange(_selectedCard, exchangedCard);
                    refreshUI = true;
                }
            }
        }
        
        if (refreshUI) {
            // update UI
            configTable(true);
            reloadCardDecks();
        }
    }
    
    // clear
    removeDragNode();
}

#pragma mark - TableViewDelegate
void BattleDeckLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    
}

void BattleDeckLayer::tableCellHighlight(TableView* table, TableViewCell* cell)
{
    _isExtracting = true;
}

#pragma mark - TableViewDataSource
Size BattleDeckLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount();
    if (idx == cnt - 1) {
        return size + Size(0, unitNodeOffsetY);
    }
    
    return size;
}

TableViewCell* BattleDeckLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    BattleDeckCell *cell = static_cast<BattleDeckCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = BattleDeckCell::create();
    }
    
    const ssize_t maxCnt = getCellsCount();
    const size_t cnt = _candidateCards.size();
    for (int i = 0; i < columnCount; ++i) {
        const ssize_t index = idx * columnCount + i;
        CardNode* unitNode = dynamic_cast<CardNode*>(cell->getNode(i));
        if (index < cnt) {
            const string& name = _candidateCards.at(index);
            const string& renderKey = getRenderKey(name);
            const UnitType* ut = _techTree->findUnitTypeByName(name);
            const int rarity = ut ? ut->getRarity() : 0;
            if (!unitNode) {
                unitNode = CardNode::create();
                unitNode->registerObserver(this);
                cell->addChild(unitNode);
                cell->setNode(unitNode, i);
            }
            
            unitNode->update(name, renderKey, rarity, 0, 10);
            unitNode->setSelected(name == _touchedCard);
            
            const Point point(_cellSize.width * (i + 0.5f) - unitNodeOffsetX / 2, unitNode->getContentSize().height * 0.5f);
            unitNode->setPosition(point + Point(0, (idx == maxCnt - 1) ? unitNodeOffsetY : 0));
        } else if (unitNode) {
            unitNode->removeFromParent();
            cell->resetNode(i);
        }
    }
    
    return cell;
}

ssize_t BattleDeckLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount();
}

#pragma mark - CardNodeObserver
void BattleDeckLayer::onCardNodeTouchedBegan(CardNode* node)
{
    _selectedCard = node->getCardName();
    createDragNode(_selectedCard);
    _dragNode->setPosition(_background->convertToNodeSpace(node->getParent()->convertToWorldSpace(node->getPosition())));
    _dragNode->setVisible(true);
}

void BattleDeckLayer::onCardNodeTouchedEnded(CardNode* node, bool isValid)
{
    removeDragNode();
    
    const string lastName = _touchedCard;
    const string& currentName = node->getCardName();
    if (lastName != currentName) {
        _touchedCard = currentName;
        
        const bool lastInDeck(_pickedCards.find(lastName) != end(_pickedCards));
        const bool currentInDeck(_pickedCards.find(currentName) != end(_pickedCards));
        
        if (lastInDeck || currentInDeck) {
            selectCardOnDecks(currentName);
        }
        
        if (!lastInDeck || !currentInDeck) {
            ssize_t lastIdx(CC_INVALID_INDEX);
            ssize_t currentIdx(CC_INVALID_INDEX);
            for (int i = 0; i < _candidateCards.size(); ++i) {
                const string& card = _candidateCards.at(i);
                if (lastName == card) {
                    lastIdx = i / columnCount;
                }
                
                if (currentName == card) {
                    currentIdx = i / columnCount;
                }
                
                if (lastIdx != CC_INVALID_INDEX && currentIdx != CC_INVALID_INDEX) {
                    break;
                }
            }
            
            if (lastIdx != CC_INVALID_INDEX) {
                _tableViewNode.table->updateCellAtIndex(lastIdx);
            }
            
            if (lastIdx != currentIdx && currentIdx != CC_INVALID_INDEX) {
                _tableViewNode.table->updateCellAtIndex(currentIdx);
            }
        }
        
        _infoNode->update(currentName, _techTree);
    }
}

#pragma mark - protected
Node* BattleDeckLayer::createTableView(const Size& size)
{
    TableViewNode& tableViewNode = _tableViewNode;
    Node* parent = Node::create();
    parent->setAnchorPoint(Point::ANCHOR_MIDDLE);
    parent->setContentSize(size);
    
    // title
    Sprite* title = CocosUtils::createTitle("", BIG_FONT_SIZE);
    tableViewNode.titleLabel = dynamic_cast<Label*>(title->getChildren().front());
    parent->addChild(title);
    
    // table view
    static const string leftFile("GameImages/public/button_sanjiao_4.png");
    Button* topButton = Button::create(leftFile, leftFile);
    parent->addChild(topButton, topZOrder);
    tableViewNode.topButton = topButton;
    
    static const string rightFile("GameImages/public/button_sanjiao_4.png");
    Button* bottomButton = Button::create(rightFile, rightFile);
    parent->addChild(bottomButton, topZOrder);
    tableViewNode.bottomButton = bottomButton;
    
    static const float offsetY(5.0f);
    const float height = size.height;
    const float titleHeight = title->getContentSize().height;
    const float topButtonHeight = topButton->getContentSize().height;
    const float bottomButtonHeight = bottomButton->getContentSize().height;
    tableViewNode.maxSize.width = size.width;
    tableViewNode.maxSize.height = height - (titleHeight + topButtonHeight + bottomButtonHeight + offsetY * 4);
    tableViewNode.topLeftPos = Point(0, height - (titleHeight + topButtonHeight + offsetY * 2));
    
    TableView* tableView = TableView::create(this, tableViewNode.maxSize);
    tableView->setDelegate(this);
    tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setBounceable(false);
    parent->addChild(tableView);
    tableViewNode.table = tableView;
    
    topButton->addClickEventListener([](Ref*) {
        
    });
    bottomButton->addClickEventListener([](Ref*) {
        
    });
    
    // set positions
    const float midX = size.width / 2;
    title->setPosition(Point(midX, height - titleHeight / 2));
    topButton->setPosition(Point(midX, height - (titleHeight + offsetY + topButtonHeight / 2)));
    bottomButton->setPosition(Point(midX, offsetY + bottomButtonHeight / 2));
    
    configTable(false);
    
    return parent;
}

ssize_t BattleDeckLayer::getCellsCount() const
{
    const size_t cnt(_candidateCards.size());
    if (cnt > 0) {
        return (cnt - 1) / columnCount + 1;
    }
    
    return 0;
}

Size BattleDeckLayer::getCellSize() const
{
    return Size(_cellSize.width * columnCount + unitNodeOffsetX, _cellSize.height);
}

cocos2d::Rect BattleDeckLayer::getRealBoundingBox(Node* node) const
{
    if (node) {
        cocos2d::Rect rect = node->getBoundingBox();
        Point origin = rect.origin;
        rect.origin = _background->convertToNodeSpace(node->getParent()->convertToWorldSpace(origin));
        return rect;
    }
    
    return cocos2d::Rect::ZERO;
}

void BattleDeckLayer::reloadTable()
{
    TableView* table = _tableViewNode.table;
    if (table) {
        const Point& offset = table->getContentOffset();
        table->reloadData();
        table->setContentOffset(offset);
    }
}

void BattleDeckLayer::configTable(bool reload)
{
    const float totalHeight = _cellSize.height * getCellsCount() + unitNodeOffsetY;
    const float maxHeight = _tableViewNode.maxSize.height;
    const Size tableSize(_tableViewNode.maxSize.width, MIN(totalHeight, maxHeight));
    const bool canScroll(maxHeight < totalHeight);
    _tableViewNode.topButton->setVisible(canScroll);
    _tableViewNode.bottomButton->setVisible(canScroll);
    _tableViewNode.titleLabel->setString(StringUtils::format("My Deck:%ld", _candidateCards.size()));
    
    TableView* table = _tableViewNode.table;
    if (table) {
        table->setViewSize(tableSize);
        table->setPosition(_tableViewNode.topLeftPos - Point(0, tableSize.height));
        if (reload) {
            reloadTable();
        }
    }
}

void BattleDeckLayer::createDragNode(const string& name)
{
    if (!_dragNode && name.length() > 0) {
        auto node = CardNode::create();
        node->update(name, getRenderKey(name), _techTree->findUnitTypeByName(name)->getRarity(), 0, 10);
        node->setVisible(false);
        _background->addChild(node, topZOrder);
        _dragNode = node;
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
            auto node = CardNode::create();
            node->update(name, renderKey, _techTree->findUnitTypeByName(name)->getRarity(), 0, 10);
            node->registerObserver(this);
            node->setTag(cardTagOnDeck);
            node->setSelected(name == _touchedCard);
            deck->addChild(node);
            const Size& size = deck->getContentSize();
            node->setPosition(Size(size.width / 2, size.height / 2));
        }
    }
    
    _selectedCardsLabel->setString(StringUtils::format("Troops:%d/%d", i, CANDIDATE_CARD_DECKS_COUNT));
}

void BattleDeckLayer::selectCardOnDecks(const string& name)
{
    for (int i = 0; i < _cardDecks.size(); ++i) {
        Sprite* deck = _cardDecks.at(i);
        CardNode* node = dynamic_cast<CardNode*>(deck->getChildByTag(cardTagOnDeck));
        if (node) {
            const string& unitName = node->getCardName();
            node->setSelected(unitName == name);
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

const set<string>& BattleDeckLayer::getPickedCards() const
{
    return _pickedCards;
}

void BattleDeckLayer::loadData()
{
    if (0 == _pickedCards.size()) {
        UserDefaultsDataManager::getInstance()->getSelectedCards(_pickedCards);
    }
    
    _candidateCards.clear();
    
    set_difference(begin(allCards), end(allCards), begin(_pickedCards), end(_pickedCards), inserter(_candidateCards, begin(_candidateCards)));
    sort(begin(_candidateCards), end(_candidateCards), sortByName);
}

void BattleDeckLayer::saveData()
{
    UserDefaultsDataManager::getInstance()->setSelectedCards(_pickedCards);
}

void BattleDeckLayer::extract(const string& name)
{
    // 1. erase
    eraseFromVector(_candidateCards, name);
    
    // 2. insert
    _pickedCards.insert(name);
}

void BattleDeckLayer::insert(const string& name)
{
    // 1. erase
    _pickedCards.erase(name);
    
    // 2. insert
    _candidateCards.push_back(name);
    sort(begin(_candidateCards), end(_candidateCards), sortByName);
}

void BattleDeckLayer::exchange(const string& picked, const string& candidate)
{
    extract(candidate);
    insert(picked);
}
