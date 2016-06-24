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

using namespace std;
using namespace ui;
using namespace UnderWorld::Core;

static const unsigned int tableColumnCount(4);
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
    auto cardSize = CardNode::create(false)->getContentSize();
    _cardSize = cardSize + Size(cardOffsetOnTable.x, cardOffsetOnTable.y);
    
    // table parameters
    const auto& winSize = getWinSize();
    {
        static const Vec2 edge(50, 70);
        _tableMaxSize.width = _cardSize.width * tableColumnCount + cardOffsetOnTable.x;
        _tableMaxSize.height = winSize.height - edge.y * 2;
        _tableBasePosition.x = winSize.width - edge.x - getCellSize().width;
        _tableBasePosition.y = winSize.height - edge.y;
    }
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
        auto cardNode = dynamic_cast<CardNode*>(cell->getNode(i));
        if (index < cnt) {
            if (!cardNode) {
                cardNode = CardNode::create(false);
                cardNode->registerObserver(this);
                cell->addChild(cardNode);
                cell->setNode(cardNode, i);
            }
            
            // we must update the position when the table was reloaded
            const Point point(_cardSize.width * (i + 0.5f) - cardOffsetOnTable.x / 2, cardNode->getContentSize().height * 0.5f);
            cardNode->setPosition(point + Point(0, (idx == maxCnt - 1) ? cardOffsetOnTable.y : 0));
            
            const string& name = cards.at(index);
            updateCardNode(cardNode, name);
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
void CardLayer::createSwitchTableButtons(const Point& position)
{
    for (int i = 0; i < tablesCount; ++i) {
        const auto normal("GameImages/formation/button_yellow_1.png");
        const auto selected("GameImages/formation/button_blue.png");
        auto button = Button::create(normal, selected, selected);
        button->addClickEventListener([this, i](Ref*) {
            setTableType(tableTypes[i]);
        });
        addChild(button);
        
        static const Vec2 offset(40.0f, 0);
        const auto& size = button->getContentSize();
        button->setPosition(position + Point((size.width + offset.x) * i + size.width / 2, -(offset.y + size.height / 2)));
        
        const auto type = tableTypes[i];
        if (_switchTableButtons.find(type) != end(_switchTableButtons)) {
            assert(false);
            _switchTableButtons.at(type) = button;
        } else {
            _switchTableButtons.insert(make_pair(type, button));
        }
    }
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
CardNode* CardLayer::createCardNode(const string& name) const
{
    auto node = CardNode::create(false);
    updateCardNode(node, name);
    return node;
}

void CardLayer::updateCardNode(CardNode* node, const string& name) const
{
    if (node && name.length() > 0) {
        int rarity(0);
        int cost(0);
        
        auto ut = DataManager::getInstance()->getTechTree()->findUnitTypeByName(name);
        if (ut) {
            rarity = ut->getRarity();
        }
        
        auto ct = DataManager::getInstance()->getGameModeHMM()->findCardTypeByName(name);
        if (ct) {
            const auto& costs = ct->getCost();
            static const auto& name(RESOURCE_NAME);
            if (costs.find(name) != costs.end()) {
                cost = costs.at(name) / GameConstants::MICRORES_PER_RES;
            }
        }
        
        node->update(name, rarity, cost, BATTLE_RESOURCE_MAX_COUNT);
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
        
        for (auto iter = begin(_switchTableButtons); iter != end(_switchTableButtons); ++iter) {
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
