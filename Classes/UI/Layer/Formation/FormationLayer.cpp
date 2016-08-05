//
//  FormationLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FormationLayer.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "TechTree.h"
#include "GameModeHMM.h"
#include "GameData.h"
#include "DataManager.h"
#include "FormationData.h"
#include "LocalHelper.h"
#include "XTableViewCell.h"
#include "FormationUnitNode.h"
#include "TabButton.h"

using namespace std;
using namespace ui;
using namespace UnderWorld::Core;

static const int topZOrder(1);
static const unsigned int deckRowCount(2);
static const unsigned int tableColumnCount(4);
static const Vec2 cardOffsetOnTable(5, 14);
static const int cardTagOnDeck(100);
static const FormationTableType tableTypes[] = {
    FormationTableType::Hero,
    FormationTableType::Spell
};
static const unsigned int tablesCount = sizeof(tableTypes) / sizeof(FormationTableType);

static inline const Size& getWinSize() { return Director::getInstance()->getWinSize(); }

FormationLayer* FormationLayer::create()
{
    FormationLayer *ret = new (nothrow) FormationLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

FormationLayer::FormationLayer()
:_observer(nullptr)
,_thisTable(nullptr)
,_cardSize(Size::ZERO)
,_tableMaxSize(Size::ZERO)
,_tableBasePosition(Point::ZERO)
,_deckBasePosition(Point::ZERO)
,_tileSize(Size::ZERO)
,_tileBasePosition(Point::ZERO)
,_draggingNode(nullptr)
,_populationLabel(nullptr)
,_spellCountLabel(nullptr)
,_thisTableType(FormationTableType::None)
,_isPickingCard(false)
,_thisFormationIdx(-1)
,_thisFormationData(nullptr)
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
    
    // deck parameters
    {
        static const Vec2 edge(60, 24);
        _deckBasePosition = edge;
    }
    
    // tile parameters
    {
        static const Vec2 edge(170, 80);
        _tileBasePosition.x = edge.x;
        _tileBasePosition.y = winSize.height - edge.y;
    }
    
    reloadAllCandidateCards();
}

FormationLayer::~FormationLayer()
{
    for (auto iter = begin(_editedFormationData); iter != end(_editedFormationData); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
    
    removeAllChildren();
    // TODO: remove the code to another place
    CocosUtils::cleanMemory();
}

void FormationLayer::registerObserver(FormationLayerObserver *observer)
{
    _observer = observer;
}

bool FormationLayer::init()
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR))
    {
        // tiles
        createTiles();
        
        // deck
        createDeck();
        
        // tables
        for (int i = 0; i < tablesCount; ++i)
        {
            auto type = tableTypes[i];
            createTable(type);
        }
        
        // buttons
        CocosUtils::createGrayExitButton(this, [this]() { removeFromParent(); });
        {
            float offsetX(-40.0f);
            static const Point basePoint(345, getWinSize().height - 15);
            createSwitchFormationButton(basePoint);
            createTabButtons(Point(_tableBasePosition.x + offsetX, basePoint.y));
            
            offsetX = 10.0f;
            createSaveFormationButton(Point(_tableBasePosition.x + offsetX, _deckBasePosition.y));
            createSetDefaultFormationButton(Point(_tableBasePosition.x + _tableMaxSize.width - offsetX, _deckBasePosition.y));
        }
        
        // labels
        updatePopulationCount(0);
        updateSpellsCount(0);
        
        setTableType(FormationTableType::Hero);
        
        const int defaultFormation = GameData::getInstance()->currentUser()->getDefaultFormationId();
        loadFormation(defaultFormation);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(FormationLayer::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(FormationLayer::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(FormationLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void FormationLayer::onEnter()
{
    LayerColor::onEnter();
}

bool FormationLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void FormationLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    if (_selectedCard.size() > 0) {
        const auto& point = touch->getLocation();
        const auto& rect = getBoundingBox(_thisTable);
        const bool isInTableView(rect.containsPoint(point));
        if (!isInTableView) {
            _thisTable->onTouchEnded(touch, unused_event);
        }
        
        if (!_draggingNode) {
            createDraggingNode(_isPickingCard ? _thisTableType : FormationTableType::Spell, _selectedCard, point);
        }
        
        if (_draggingNode) {
            _draggingNode->setVisible(!isInTableView);
            _draggingNode->setPosition(point);
        }
    }
}

void FormationLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_selectedCard.size() > 0 && _draggingNode) {
        const auto& point = touch->getLocation();
        static const auto spellTableType(FormationTableType::Spell);
        FormationTableType type = _isPickingCard ? _thisTableType : spellTableType;
        if (FormationTableType::Hero == type) {
            onPlacedEnded(_selectedCard, point);
        } else if (spellTableType == type) {
            const ssize_t idx = getIntersectedDeckIdx(_draggingNode->getBoundingBox());
            bool refreshUI(false);
            auto spellTable = _tables.at(spellTableType);
            if (_isPickingCard) {
                if (CC_INVALID_INDEX != idx) {
                    auto cnt = _thisFormationData->getSpells().size();
                    if (cnt < FORMATION_SPELLS_COUNT) {
                        pickSpellCard(_selectedCard);
                        refreshUI = true;
                    } else if (_decks.size() > idx) {
                        auto node = dynamic_cast<CardNode*>(_decks.at(idx)->getChildByTag(cardTagOnDeck));
                        if (node) {
                            exchangeSpellCards(node->getCardName(), _selectedCard);
                            refreshUI = true;
                        }
                    }
                }
                
                _isPickingCard = false;
            } else {
                // pick card from spell deck to table
                if (CC_INVALID_INDEX == idx) {
                    cancelSpellCard(_selectedCard);
                    refreshUI = true;
                } else {
                    string exchanged("");
                    const auto& rect = getBoundingBox(spellTable);
                    if (rect.containsPoint(point)) {
                        exchanged = "";
                    }
                    
                    if (exchanged.size() > 0) {
                        exchangeSpellCards(_selectedCard, exchanged);
                        refreshUI = true;
                    }
                }
            }
            
            if (refreshUI) {
                refreshTable(spellTable, true);
                reloadDecks();
            }
        }
    }
    
    // clear
    removeDraggingNode();
}

#pragma mark - TableViewDelegate
void FormationLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    
}

void FormationLayer::tableCellHighlight(TableView* table, TableViewCell* cell)
{
    _isPickingCard = true;
}

#pragma mark - TableViewDataSource
Size FormationLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount(table);
    if (idx == cnt - 1) {
        return size + Size(0, cardOffsetOnTable.y);
    }
    
    return size;
}

TableViewCell* FormationLayer::tableCellAtIndex(TableView *table, ssize_t idx)
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

ssize_t FormationLayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount(table);
}

#pragma mark - CardNodeObserver
void FormationLayer::onCardNodeTouchedBegan(CardNode* node)
{
    _selectedCard = node->getCardName();
    const auto& point = convertToNodeSpace(node->getParent()->convertToWorldSpace(node->getPosition()));
    createDraggingNode(isContainedByTableView(point) ? _thisTableType : FormationTableType::Spell, _selectedCard, point);
    const auto& spells = _thisFormationData->getSpells();
    for (int i = 0; i < spells.size(); ++i) {
        const auto& name = spells.at(i);
        if (name == _selectedCard) {
            _draggingNode->setVisible(true);
            break;
        }
    }
}

void FormationLayer::onCardNodeTouchedEnded(CardNode* node, bool isValid)
{
    removeDraggingNode();
}

#pragma mark - protected
#pragma mark tiles
void FormationLayer::createTiles()
{
    for (int j = 0; j < FORMATION_HEIGHT; ++j) {
        for (int i = 0; i < FORMATION_WIDTH; ++i) {
            const auto idx = j * FORMATION_WIDTH + i;
            const auto file = (0 == idx % 2) ? "GameImages/formation/ui_gezi48x48.png" : "GameImages/formation/ui_gezi48x48_1.png";
            auto node = Sprite::create(file);
            addChild(node);
            
            const auto& size = node->getContentSize();
            node->setPosition(_tileBasePosition + Point(size.width * (i + 0.5f), -(size.height * (j + 0.5f))));
            
            TileInfo info;
            info.idx = idx;
            info.node = node;
            info.midPoint = node->getPosition();
            _tiles.push_back(info);
            
            if (0 == _tileSize.width) {
                _tileSize = node->getContentSize();
            }
        }
    }
}

#pragma mark deck
void FormationLayer::createDeck()
{
    const unsigned int column(FORMATION_SPELLS_COUNT / deckRowCount);
    for (int j = 0; j < deckRowCount; ++j) {
        for (int i = 0; i < column; ++i) {
            auto sprite = Sprite::create("GameImages/test/ui_jiabing.png");
            addChild(sprite);
            _decks.push_back(sprite);
            
            const auto& size = sprite->getContentSize();
            static const Vec2 offset(9, 17);
            sprite->setPosition(_deckBasePosition + Point((i + 0.5) * size.width + i * offset.x, (j + 0.5) * size.height + j * offset.y));
        }
    }
}

#pragma mark table
void FormationLayer::createTable(FormationTableType type)
{
    auto tableView = TableView::create(this, _tableMaxSize);
    tableView->setDelegate(this);
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

void FormationLayer::refreshTable(TableView* table, bool reload)
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

ssize_t FormationLayer::getCellsCount(TableView* table) const
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

Size FormationLayer::getCellSize() const
{
    return Size(_tableMaxSize.width, _cardSize.height);
}

bool FormationLayer::isContainedByTableView(const Point& point)
{
    if (_tables.size() > 0) {
        auto table = _tables.begin()->second;
        if (table) {
            const auto& size = getBoundingBox(table);
            return size.containsPoint(point);
        }
    }
    
    return false;
}

Rect FormationLayer::getBoundingBox(Node* node) const
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
void FormationLayer::createSwitchFormationButton(const Point& position)
{
    for (int i = 0; i < FORMATION_MAX_COUNT; ++i) {
        static const auto normal("GameImages/formation/button_yellow_1.png");
        static const auto selected("GameImages/formation/button_blue.png");
        const string title = StringUtils::format("%d", i + 1);
        auto button = TabButton::create(title, normal, selected, [this, i](Ref*) {
            loadFormation(i);
        });
        addChild(button);
        _switchFormationButtons.push_back(button);
        
        static const Vec2 offset(10.0f, 0);
        const auto& size = button->getContentSize();
        button->setPosition(position + Point((size.width + offset.x) * i + size.width / 2, - (offset.y + size.height / 2)));
    }
}

void FormationLayer::createTabButtons(const Point& position)
{
    for (int i = 0; i < tablesCount; ++i) {
        const auto type = tableTypes[i];
        const string title = getTableName(type);
        auto button = TabButton::create(title, [this, i](Ref*) {
            setTableType(tableTypes[i]);
        });
        addChild(button);
        
        static const Vec2 offset(10.0f, 0);
        const auto& size = button->getContentSize();
        button->setPosition(position + Point((size.width + offset.x) * i + size.width / 2, -(offset.y + size.height / 2)));
        
        if (_tabButtons.find(type) != end(_tabButtons)) {
            assert(false);
            _tabButtons.at(type) = button;
        } else {
            _tabButtons.insert(make_pair(type, button));
        }
    }
}

void FormationLayer::createSaveFormationButton(const Point& position)
{
    static const string file("GameImages/formation/f_b_save.png");
    auto button = Button::create(file, file);
    button->addClickEventListener([this](Ref*) {
        saveThisFormation();
    });
    addChild(button);
    
    const auto& size = button->getContentSize();
    button->setPosition(position + Point(size.width / 2, size.height / 2));
}

void FormationLayer::createSetDefaultFormationButton(const Point& position)
{
    static const string file("GameImages/formation/f_b_default.png");
    auto button = Button::create(file, file);
    button->addClickEventListener([this](Ref*) {
        setDefaultFormation();
    });
    addChild(button);
    
    const auto& size = button->getContentSize();
    button->setPosition(position + Point(-size.width / 2, size.height / 2));
}

#pragma mark labels
void FormationLayer::updatePopulationCount(int count)
{
    const string msg = StringUtils::format("%d/%d", count, FORMATION_POPULATION_COUNT);
    if (!_populationLabel) {
        auto hintLabel = CocosUtils::createLabel(LocalHelper::getString("ui_formation_heroResource"), DEFAULT_FONT_SIZE);
        addChild(hintLabel);
        
        _populationLabel = CocosUtils::createLabel(msg, DEFAULT_FONT_SIZE);
        addChild(_populationLabel);
        
        // set positions
        const auto& hintSize = hintLabel->getContentSize();
        {
            static const Point offset(0, 10.0f);
            hintLabel->setPosition(Point(_deckBasePosition.x + hintSize.width / 2, _tileBasePosition.y + hintSize.height / 2) + offset);
        }
        
        {
            const auto& resourceSize = _populationLabel->getContentSize();
            static const Point offset(5.0, 0);
            _populationLabel->setPosition(hintLabel->getPosition() + Point((hintSize.width + resourceSize.width) / 2, 0) + offset);
        }
    } else {
        _populationLabel->setString(msg);
    }
}

void FormationLayer::updateSpellsCount(size_t count)
{
    const string msg = StringUtils::format("%ld/%d", count, FORMATION_SPELLS_COUNT);
    if (!_spellCountLabel) {
        auto hintLabel = CocosUtils::createLabel(LocalHelper::getString("ui_formation_spellResource"), DEFAULT_FONT_SIZE);
        addChild(hintLabel);
        
        _spellCountLabel = CocosUtils::createLabel(msg, DEFAULT_FONT_SIZE);
        addChild(_spellCountLabel);
        
        // set positions
        const auto& hintSize = hintLabel->getContentSize();
        {
            static const Point offset(0, 250.0f);
            hintLabel->setPosition(_deckBasePosition + Point(hintSize.width / 2, hintSize.height / 2) + offset);
        }
        
        {
            const auto& resourceSize = _spellCountLabel->getContentSize();
            static const Point offset(5.0, 0);
            _spellCountLabel->setPosition(hintLabel->getPosition() + Point((hintSize.width + resourceSize.width) / 2, 0) + offset);
        }
    } else {
        _spellCountLabel->setString(msg);
    }
}

#pragma mark card
CardNode* FormationLayer::createCardNode(const string& name) const
{
    auto node = CardNode::create(false);
    updateCardNode(node, name);
    return node;
}

void FormationLayer::updateCardNode(CardNode* node, const string& name) const
{
    if (node && name.length() > 0) {
        node->update(name, DataManager::getInstance()->getBattleResourceMaxCount());
    }
}

void FormationLayer::createDraggingNode(FormationTableType type, const string& name, const Point& point)
{
    if (!_draggingNode && name.length() > 0) {
        Node* node(nullptr);
        if (FormationTableType::Hero == type) {
            node = Sprite::create("GameImages/effects/backcircle_bule.png");
        } else {
            node = createCardNode(name);
        }
        node->setVisible(false);
        node->setPosition(point);
        addChild(node, topZOrder);
        _draggingNode = node;
    }
}

void FormationLayer::removeDraggingNode()
{
    if (_draggingNode) {
        _draggingNode->removeFromParent();
        _draggingNode = nullptr;
        _selectedCard = "";
    }
}

void FormationLayer::unitBackToTable()
{
    removeDraggingNode();
}

void FormationLayer::unitBackToFormation()
{
    
}

FormationUnitNode* FormationLayer::createUnitNode(const string& name)
{
    string renderKey(name);
    auto ut = DataManager::getInstance()->getTechTree()->findUnitTypeByName(name);
    if (ut) {
        renderKey = ut->getRenderKey();
    }
    
    auto node = FormationUnitNode::create(name, renderKey, _tileSize);
    addChild(node);
    node->setTouchEnabled(true);
    node->setSwallowTouches(true);
    node->addTouchEventListener([this](Ref* ref, Widget::TouchEventType type) {
        auto node = dynamic_cast<FormationUnitNode*>(ref);
        if (Widget::TouchEventType::BEGAN == type) {
            onUnitTouchedBegan(node);
        } else if (Widget::TouchEventType::MOVED == type) {
            onUnitTouchedMoved(node);
        } else if (Widget::TouchEventType::ENDED == type ||
                   Widget::TouchEventType::CANCELED == type) {
            onUnitTouchedEnded(node);
        }
    });
    
    return node;
}

#pragma mark deck
ssize_t FormationLayer::getIntersectedDeckIdx(const Rect& rect) const
{
    float intersectedArea(0);
    ssize_t idx(CC_INVALID_INDEX);
    for (int i = 0; i < _decks.size(); ++i) {
        const Rect& bd = _decks.at(i)->getBoundingBox();
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

void FormationLayer::pickSpellCard(const string& name)
{
    static const auto type(FormationTableType::Spell);
    removeCandidateCard(type, name);
    _thisFormationData->insertSpell(name);
}

void FormationLayer::cancelSpellCard(const string& name)
{
    static const auto type(FormationTableType::Spell);
    _thisFormationData->removeSpell(name);
    insertCandidateCard(type, name);
    auto& cards = _candidateCards.at(type);
    sort(begin(cards), end(cards), [](const string& first, const string& second){
        return (first > second);
    });
}

void FormationLayer::exchangeSpellCards(const string& picked, const string& candidate)
{
    pickSpellCard(candidate);
    cancelSpellCard(picked);
}

void FormationLayer::reloadDecks()
{
    const size_t cnt = _decks.size();
    // remove all
    for (int i = 0; i < cnt; ++i) {
        auto deck = _decks.at(i);
        auto node = deck->getChildByTag(cardTagOnDeck);
        if (node) {
            deck->removeChild(node, true);
        }
    }
    // add
    const auto& spells = _thisFormationData->getSpells();
    for (int i = 0; i < spells.size(); ++i) {
        if (cnt > i) {
            auto deck = _decks.at(i);
            const auto& name = spells.at(i);
            auto node = createCardNode(name);
            node->registerObserver(this);
            node->setTag(cardTagOnDeck);
            node->setSelected(name == _touchedCard);
            deck->addChild(node);
            const Size& size = deck->getContentSize();
            node->setPosition(Size(size.width / 2, size.height / 2));
        }
    }
    
    updateSpellsCount(spells.size());
}

void FormationLayer::selectCardOnDecks(const string& name)
{
    for (int i = 0; i < _decks.size(); ++i) {
        Sprite* deck = _decks.at(i);
        CardNode* node = dynamic_cast<CardNode*>(deck->getChildByTag(cardTagOnDeck));
        if (node) {
            const string& unitName = node->getCardName();
            node->setSelected(unitName == name);
        }
    }
}

#pragma mark functions
void FormationLayer::reloadAllCandidateCards()
{
    _candidateCards.clear();
    auto dm = DataManager::getInstance();
    const auto& cards = dm->getCardDecks();
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        const auto& name = *iter;
        auto ct = dm->getGameModeHMM()->findCardTypeByName(name);
        if (ct) {
            if (kHMMCardClass_Spell == ct->getCardClass()) {
                insertCandidateCard(FormationTableType::Spell, name);
            } else {
                insertCandidateCard(FormationTableType::Hero, name);
            }
        } else {
            // TODO: remove code
            insertCandidateCard(FormationTableType::Hero, name);
            insertCandidateCard(FormationTableType::Spell, name);
        }
    }
}

void FormationLayer::reloadCandidateCards(FormationTableType type)
{
    if (_candidateCards.find(type) != end(_candidateCards)) {
        _candidateCards.at(type).clear();
    }
    
    auto dm = DataManager::getInstance();
    const auto& cards = dm->getCardDecks();
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        const auto& name = *iter;
        auto ct = dm->getGameModeHMM()->findCardTypeByName(name);
        if (ct) {
            if (kHMMCardClass_Spell == ct->getCardClass()) {
                if (FormationTableType::Spell == type) {
                    insertCandidateCard(type, name);
                }
            } else if (FormationTableType::Hero == type) {
                insertCandidateCard(type, name);
            }
        } else {
            // TODO: remove code
            insertCandidateCard(type, name);
        }
    }
}

void FormationLayer::insertCandidateCard(FormationTableType type, const string& name)
{
    if (_candidateCards.find(type) == end(_candidateCards)) {
        _candidateCards.insert(make_pair(type, vector<string>()));
    }
    
    auto& cards = _candidateCards.at(type);
    cards.push_back(name);
}

void FormationLayer::removeCandidateCard(FormationTableType type, const string& name)
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

FormationTableType FormationLayer::getTableType(TableView* table) const
{
    if (table) {
        const size_t cnt = _tables.size();
        if (cnt < tablesCount) {
            return tableTypes[cnt];
        }
        
        return static_cast<FormationTableType>(table->getTag());
    }
    
    return FormationTableType::None;
}

void FormationLayer::setTableType(FormationTableType type)
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

string FormationLayer::getTableName(FormationTableType type) const
{
    switch (type) {
        case FormationTableType::Hero:
            return LocalHelper::getString("ui_formation_tab_hero");
        case FormationTableType::Spell:
            return LocalHelper::getString("ui_formation_tab_spell");
        default:
            return "";
    }
}

Coordinate32 FormationLayer::formationIdx2Coordinate(int idx) const
{
    return Coordinate32(idx % FORMATION_WIDTH, FORMATION_HEIGHT - idx / FORMATION_WIDTH - 1);
}

int FormationLayer::formationCoordinate2Idx(const Coordinate32& point) const
{
    return FORMATION_WIDTH * (FORMATION_HEIGHT - point.y - 1) + point.x;
}

void FormationLayer::saveThisFormation()
{
    auto user = GameData::getInstance()->currentUser();
    user->setFormationData(_thisFormationIdx, _thisFormationData);
    user->saveFormationData(_thisFormationIdx);
}

void FormationLayer::loadFormation(int idx)
{
    if (_thisFormationIdx != idx) {
        _thisFormationIdx = idx;
        
        for (int i = 0; i < _switchFormationButtons.size(); ++i) {
            auto button = _switchFormationButtons.at(i);
            if (button) {
                button->setEnabled(idx != i);
            }
        }
        
        if (_editedFormationData.find(idx) == end(_editedFormationData)) {
            auto fd = GameData::getInstance()->currentUser()->getFormationData(idx);
            _editedFormationData.insert(make_pair(idx, new (nothrow) FormationData(fd)));
        }
        
        _thisFormationData = _editedFormationData.at(idx);
        
        // 1. heroes
        for (auto iter = begin(_formationNodes); iter != end(_formationNodes); ++iter) {
            iter->second->removeFromParent();
        }
        _formationNodes.clear();
        
        const auto& heroes = _thisFormationData->getHeroes();
        for (auto iter = begin(heroes); iter != end(heroes); ++iter) {
            const int idx = formationCoordinate2Idx(iter->first);
            if (idx >= 0 && idx < FORMATION_WIDTH * FORMATION_HEIGHT) {
                auto node = createUnitNode(iter->second);
                _formationNodes.insert(make_pair(idx, node));
                const auto& tileInfo = _tiles.at(idx);
                placeUnit(node, tileInfo.midPoint);
            }
        }
        
        // 2. spells
        static const auto type(FormationTableType::Spell);
        reloadCandidateCards(type);
        const auto& spells = _thisFormationData->getSpells();
        for (int i = 0; i < spells.size(); ++i) {
            removeCandidateCard(type, spells.at(i));
        }
        
        // update UI
        reloadDecks();
        for (auto iter = begin(_tables); iter != end(_tables); ++iter) {
            refreshTable(iter->second, true);
        }
    }
}

void FormationLayer::setDefaultFormation()
{
    GameData::getInstance()->currentUser()->setDefaultFormationId(_thisFormationIdx);
}

void FormationLayer::placeUnit(FormationUnitNode* node, const Point& point)
{
    if (node) {
        node->setPosition(point);
        node->setLocalZOrder(getUnitZOrder(point));
    }
}

void FormationLayer::onPlacedEnded(const string& name, const Point& point)
{
    const auto& tileInfoB = getTileInfo(point);
    int idxB(tileInfoB.idx);
    if (idxB >= 0) {
        // create node
        auto nodeA = createUnitNode(name);
        
        // if the position is occupied
        if (_formationNodes.find(idxB) != end(_formationNodes)) {
            _formationNodes.at(idxB)->removeFromParent();
            _formationNodes.at(idxB) = nodeA;
        } else {
            _formationNodes.insert(make_pair(idxB, nodeA));
        }
        
        const auto& coordinate = formationIdx2Coordinate(idxB);
        _thisFormationData->removeHero(coordinate);
        _thisFormationData->insertHero(coordinate, name);
        
        // place
        placeUnit(nodeA, tileInfoB.midPoint);
    }
}

void FormationLayer::onUnitTouchedBegan(FormationUnitNode* node)
{
    if (node) {
        const auto& point = node->getTouchBeganPosition();
        createDraggingNode(FormationTableType::Hero, node->getUnitName(), point);
    }
}

void FormationLayer::onUnitTouchedMoved(FormationUnitNode* node)
{
    if (node && _draggingNode) {
        _draggingNode->setVisible(true);
        _draggingNode->setPosition(node->getTouchMovePosition());
    }
}

void FormationLayer::onUnitTouchedEnded(FormationUnitNode* nodeA)
{
    if (nodeA) {
        const auto& tileInfoA = getTileInfo(nodeA->getTouchBeganPosition());
        const int idxA(tileInfoA.idx);
        if (idxA >= 0 && _formationNodes.find(idxA) != end(_formationNodes)) {
            const auto fcA = formationIdx2Coordinate(idxA);
            const auto& tileInfoB = getTileInfo(nodeA->getTouchEndPosition());
            int idxB(tileInfoB.idx);
            if (idxB < 0) {
                nodeA->removeFromParent();
                _formationNodes.erase(idxA);
                
                _thisFormationData->removeHero(fcA);
            } else if (idxA != idxB) {
                const auto fcB = formationIdx2Coordinate(idxB);
                const auto& pointB = tileInfoB.midPoint;
                // if the position is occupied
                if (_formationNodes.find(idxB) != end(_formationNodes)) {
                    auto nodeB(_formationNodes.at(idxB));
                    
                    // exchange
                    if (nodeB != nodeA) {
                        const auto& pointA = tileInfoA.midPoint;
                        placeUnit(nodeA, pointB);
                        placeUnit(nodeB, pointA);
                        
                        _formationNodes.at(idxA) = nodeB;
                        _formationNodes.at(idxB) = nodeA;
                        
                        _thisFormationData->exchangeHero(fcA, fcB);
                    }
                } else {
                    placeUnit(nodeA, pointB);
                    
                    _formationNodes.insert(make_pair(idxB, nodeA));
                    _formationNodes.erase(idxA);
                    
                    _thisFormationData->removeHero(fcA);
                    _thisFormationData->insertHero(fcB, nodeA->getUnitName());
                }
            }
        } else {
            assert(false);
        }
        
        removeDraggingNode();
    }
}

const FormationLayer::TileInfo& FormationLayer::getTileInfo(const Point& point) const
{
    int x = (point.x - _tileBasePosition.x) / _tileSize.width;
    int y = (_tileBasePosition.y - point.y) / _tileSize.height;
    if (x < FORMATION_WIDTH && y < FORMATION_HEIGHT) {
        const int idx = y * FORMATION_WIDTH + x;
        if (idx < _tiles.size()) {
            return _tiles.at(idx);
        }
    }
    
    static TileInfo nullInfo;
    return nullInfo;
}

int FormationLayer::getUnitZOrder(const Point& point) const
{
    static const int zorder_min(1000);
    const Point firstMidPos = _tileBasePosition + Point(_tileSize.width / 2, -_tileSize.height / 2);
    const int idx = (firstMidPos.y - point.y) / _tileSize.height * FORMATION_WIDTH + (point.x - firstMidPos.x) / _tileSize.width;
    
    return zorder_min + idx;
}
