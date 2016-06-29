//
//  CardLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/24.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "TechTree.h"
#include "GameModeHMM.h"
#include "DataManager.h"
#include "XTableViewCell.h"
#include "TabButton.h"

using namespace std;
using namespace ui;
using namespace UnderWorld::Core;

static const unsigned int tableColumnCount(10);
static const Vec2 cardOffsetOnTable(5, 14);
static const CardTableType tableTypes[] = {
    CardTableType::All,
    CardTableType::Heroes,
    CardTableType::Soldiers,
    CardTableType::Spells
};
static const unsigned int tablesCount = sizeof(tableTypes) / sizeof(CardTableType);

static inline const Size& getWinSize() { return Director::getInstance()->getWinSize(); }

CardLayer* CardLayer::create()
{
    CardLayer *ret = new (nothrow) CardLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardLayer::CardLayer()
:_observer(nullptr)
,_thisTableType(static_cast<CardTableType>(-1))
,_thisTable(nullptr)
,_cardSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
,_cardsCountLabel(nullptr)
{
    auto cardSize = XCardNode::create()->getContentSize();
    _cardSize = cardSize + Size(cardOffsetOnTable.x, cardOffsetOnTable.y);
    
    // table parameters
    const auto& winSize = getWinSize();
    {
        static const Vec2 edge(50, 100);
        _tableMaxSize.width = _cardSize.width * tableColumnCount + cardOffsetOnTable.x;
        _tableMaxSize.height = winSize.height - edge.y;
        _tableBasePosition.x = winSize.width - edge.x - getCellSize().width;
        _tableBasePosition.y = winSize.height - edge.y;
    }
    
    reloadAllCandidateCards();
}

CardLayer::~CardLayer()
{
    removeAllChildren();
}

void CardLayer::registerObserver(CardLayerObserver *observer)
{
    _observer = observer;
}

bool CardLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        // tables
        for (int i = 0; i < tablesCount; ++i) {
            auto type = tableTypes[i];
            createTableView(type);
        }
        
        // buttons
        CocosUtils::createExitButton(this, [this]() { removeFromParent(); });
        {
            const auto& winSize = getWinSize();
            static const Vec2 edge(180, 50);
            createTabButtons(Point(edge.x, winSize.height - edge.y));
        }
        
        // labels
        updateCardsCount(0);
        
        setTableType(CardTableType::All);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [](Touch *pTouch, Event *pEvent) { return true; };
        eventListener->onTouchEnded = [](Touch *pTouch, Event *pEvent) {};
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void CardLayer::onEnter()
{
    LayerColor::onEnter();
}

#pragma mark - TableViewDataSource
Size CardLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount(table);
    if (idx == cnt - 1) {
        return size + Size(0, cardOffsetOnTable.y);
    }
    
    return size;
}

TableViewCell* CardLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    const ssize_t maxCnt = getCellsCount(table);
    const auto& cards = _candidateCards.at(getTableType(table));
    const size_t cnt = cards.size();
    for (int i = 0; i < tableColumnCount; ++i) {
        const ssize_t index = idx * tableColumnCount + i;
        auto cardNode = dynamic_cast<XCardNode*>(cell->getNode(i));
        if (index < cnt) {
            if (!cardNode) {
                cardNode = XCardNode::create();
                cardNode->getCardNode()->registerObserver(this);
                cell->addChild(cardNode);
                cell->setNode(cardNode, i);
            }
            
            // we must update the position when the table was reloaded
            const Point point(_cardSize.width * (i + 0.5f) - cardOffsetOnTable.x / 2, cardNode->getContentSize().height * 0.5f);
            cardNode->setPosition(point + Point(0, (idx == maxCnt - 1) ? cardOffsetOnTable.y : 0));
            
            const string& name = cards.at(index);
            updateXCardNode(cardNode, name);
        } else if (cardNode) {
            cardNode->removeFromParent();
            cell->resetNode(i);
        }
    }
    
    return cell;
}

ssize_t CardLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount(table);
}

#pragma mark - CardNodeObserver
void CardLayer::onCardNodeTouchedBegan(CardNode* node)
{
    
}

void CardLayer::onCardNodeTouchedEnded(CardNode* node, bool isValid)
{
    if (node) {
        const auto& name(node->getCardName());
        if (CardTableType::Heroes == _thisTableType) {
            auto layer = CardXInfoLayer::create();
            layer->update(name);
            layer->registerObserver(this);
            addChild(layer);
        } else {
            auto layer = CardInfoLayer::create();
            layer->update(name);
            layer->registerObserver(this);
            addChild(layer);
        }
    }
}

#pragma mark - CardInfoLayerObserver
void CardLayer::onCardInfoLayerClosed(const string& name)
{
    
}

void CardLayer::onCardInfoLayerUpgradeCard(const string& name)
{
    
}

#pragma mark - CardXInfoLayerObserver
void CardLayer::onCardXInfoLayerClosed(const string& name)
{
    
}

void CardLayer::onCardXInfoLayerUpgradeCard(const string& name)
{
    
}

void CardLayer::onCardXInfoLayerUpgradeTalent(const string& name)
{
    
}

#pragma mark table
void CardLayer::createTableView(CardTableType type)
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

void CardLayer::refreshTable(TableView* table, bool reload)
{
    if (table) {
        auto totalHeight = _cardSize.height * getCellsCount(table) + cardOffsetOnTable.y;
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

ssize_t CardLayer::getCellsCount(TableView* table) const
{
    auto type = getTableType(table);
    if (_candidateCards.find(type) != end(_candidateCards)) {
        const auto& cards = _candidateCards.at(type);
        const size_t cnt(cards.size());
        if (cnt > 0) {
            return (cnt - 1) / tableColumnCount + 1;
        }
    }
    
    return 0;
}

Size CardLayer::getCellSize() const
{
    return Size(_tableMaxSize.width, _cardSize.height);
}

Rect CardLayer::getBoundingBox(Node* node) const
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
void CardLayer::createTabButtons(const Point& position)
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

#pragma mark labels
void CardLayer::updateCardsCount(int count)
{
    const string msg = StringUtils::format("%d/%d", count, FORMATION_POPULATION_COUNT);
    if (!_cardsCountLabel) {
        auto hintLabel = CocosUtils::createLabel("收集进度：", DEFAULT_FONT_SIZE);
        addChild(hintLabel);
        
        _cardsCountLabel = CocosUtils::createLabel(msg, DEFAULT_FONT_SIZE);
        addChild(_cardsCountLabel);
        
        // set positions
        const auto& hintSize = hintLabel->getContentSize();
        {
            static const Point offset(0, 10.0f);
            hintLabel->setPosition(Point(_tableBasePosition.x + hintSize.width / 2, _tableBasePosition.y + hintSize.height / 2) + offset);
        }
        
        {
            const auto& resourceSize = _cardsCountLabel->getContentSize();
            static const Point offset(5.0, 0);
            _cardsCountLabel->setPosition(hintLabel->getPosition() + Point((hintSize.width + resourceSize.width) / 2, 0) + offset);
        }
    } else {
        _cardsCountLabel->setString(msg);
    }
}

#pragma mark card
XCardNode* CardLayer::createXCardNode(const string& name) const
{
    auto node = XCardNode::create();
    updateXCardNode(node, name);
    return node;
}

void CardLayer::updateXCardNode(XCardNode* node, const string& name) const
{
    if (node && name.length() > 0) {
        node->getCardNode()->update(name, BATTLE_RESOURCE_MAX_COUNT);
    }
}

#pragma mark functions
void CardLayer::reloadAllCandidateCards()
{
    _candidateCards.clear();
    auto dm = DataManager::getInstance();
    const auto& cards = dm->getCardDecks();
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        const auto& name = *iter;
        auto ct = dm->getGameModeHMM()->findCardTypeByName(name);
        if (ct) {
            if (kHMMCardClass_Spell == ct->getCardClass()) {
                insertCandidateCard(CardTableType::Spells, name);
            } else {
                insertCandidateCard(CardTableType::Heroes, name);
            }
        } else {
            // TODO: remove code
            insertCandidateCard(CardTableType::All, name);
            insertCandidateCard(CardTableType::Heroes, name);
            insertCandidateCard(CardTableType::Soldiers, name);
            insertCandidateCard(CardTableType::Spells, name);
        }
    }
}

void CardLayer::reloadCandidateCards(CardTableType type)
{
    if (_candidateCards.find(type) != end(_candidateCards)) {
        _candidateCards.at(type).clear();
    }
    
    auto dm = DataManager::getInstance();
    const auto& cards = dm->getCardDecks();
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        const auto& name = *iter;
        auto ct = dm->getGameModeHMM()->findCardTypeByName(name);
        if (kHMMCardClass_Spell == ct->getCardClass()) {
            if (CardTableType::Spells == type) {
                insertCandidateCard(type, name);
            }
        } else if (CardTableType::Heroes == type) {
            insertCandidateCard(type, name);
        }
    }
}


void CardLayer::insertCandidateCard(CardTableType type, const string& name)
{
    if (_candidateCards.find(type) == end(_candidateCards)) {
        _candidateCards.insert(make_pair(type, vector<string>()));
    }
    
    auto& cards = _candidateCards.at(type);
    cards.push_back(name);
}

void CardLayer::removeCandidateCard(CardTableType type, const string& name)
{
    if (_candidateCards.find(type) != end(_candidateCards)) {
        auto& cards = _candidateCards.at(type);
        for (auto iter = begin(cards); iter != end(cards); ++iter) {
            if (name == *iter) {
                cards.erase(iter);
                break;
            }
        }
    }
}

CardTableType CardLayer::getTableType(TableView* table) const
{
    if (table) {
        const size_t cnt = _tables.size();
        if (cnt < tablesCount) {
            return tableTypes[cnt];
        }
        
        return static_cast<CardTableType>(table->getTag());
    }
    
    return static_cast<CardTableType>(-1);
}

void CardLayer::setTableType(CardTableType type)
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

string CardLayer::getTableName(CardTableType type) const
{
    switch (type) {
        case CardTableType::All:
            return "全部";
        case CardTableType::Heroes:
            return "布阵英雄";
        case CardTableType::Soldiers:
            return "援军法术";
        case CardTableType::Spells:
            return "法术卡牌";
        default:
            return "";
    }
}
