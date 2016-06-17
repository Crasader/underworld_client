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
#include "DataManager.h"
#include "FormationCell.h"
#include "FormationUnitNode.h"

using namespace std;
using namespace ui;

static const unsigned int columnCount(4);
static const Vec2 cardNodeOffset(5, 14);
static const int topZOrder(1);

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
,_table(nullptr)
,_tileSize(Size::ZERO)
,_tileBasePosition(Size::ZERO)
,_draggingNode(nullptr)
,_populationLabel(nullptr)
,_spellCountLabel(nullptr)
{
    auto cardSize = CardNode::create(false)->getContentSize();
    _cardSize = cardSize + Size(cardNodeOffset.x, cardNodeOffset.y);
    
    // table parameters
    const auto& winSize = getWinSize();
    {
        static const float edgeX(50.0f);
        static const float edgeY(50.0f);
        _tableMaxSize.width = _cardSize.width * columnCount + cardNodeOffset.x;
        _tableMaxSize.height = winSize.height - edgeY * 2;
        _tableBasePosition.x = winSize.width - edgeX - getCellSize().width;
        _tableBasePosition.y = winSize.height - edgeY;
    }
    
    // tile parameters
    {
        static const float edgeX(170.0f);
        static const float edgeY(80.0f);
        _tileBasePosition.x = edgeX;
        _tileBasePosition.y = winSize.height - edgeY;
    }
    
    _techTree = new (nothrow) TechTree();
    _techTree->init(DataManager::getInstance()->getTechTreeData());
    
    _gameModeHMM = new (nothrow) GameModeHMM();
    
    const auto& cards = DataManager::getInstance()->getCardDecks();
    for (auto iter = begin(cards); iter != end(cards); ++iter) {
        _candidateCards.push_back(*iter);
    }
    
    CocosUtils::loadPVR("pangzi");
}

FormationLayer::~FormationLayer()
{
    removeAllChildren();
    CC_SAFE_DELETE(_techTree);
    CC_SAFE_DELETE(_gameModeHMM);
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
        createTableView();
        
        // buttons
        createExitButton();
        createSwitchFormationButton();
        
        static const float edgeX(60);
        const float y = (_tileBasePosition.y - FORMATION_HEIGHT * _tileSize.height) / 2;
        createSaveFormationButton(Point(_tileBasePosition.x + edgeX, y));
        createSetDefaultFormationButton(Point(_tileBasePosition.x + FORMATION_WIDTH * _tileSize.width - edgeX, y));
        
        // labels
        updatePopulationCount(10);
        
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
        const auto& rect = getBoundingBox(_table);
        const bool isInTableView(rect.containsPoint(point));
        if (!isInTableView) {
            _table->onTouchEnded(touch, unused_event);
        }
        
        if (!_draggingNode) {
            createDraggingNode(_selectedCard, point);
        }
        
        if (_draggingNode) {
            _draggingNode->setVisible(!isInTableView);
            _draggingNode->setPosition(point);
        }
    }
}

void FormationLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_selectedCard.size() > 0) {
        const auto& point = touch->getLocation();
        onPlacedEnded(_selectedCard, point);
        _selectedCard = "";
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
    
}

#pragma mark - TableViewDataSource
Size FormationLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const Size size = getCellSize();
    const ssize_t cnt = getCellsCount();
    if (idx == cnt - 1) {
        return size + Size(0, cardNodeOffset.y);
    }
    
    return size;
}

TableViewCell* FormationLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    auto cell = static_cast<FormationCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = FormationCell::create();
    }
    
    const ssize_t maxCnt = getCellsCount();
    const size_t cnt = _candidateCards.size();
    for (int i = 0; i < columnCount; ++i) {
        const ssize_t index = idx * columnCount + i;
        auto cardNode = dynamic_cast<CardNode*>(cell->getNode(i));
        if (index < cnt) {
            if (!cardNode) {
                cardNode = CardNode::create(false);
                cardNode->registerObserver(this);
                cell->addChild(cardNode);
                cell->setNode(cardNode, i);
            }
            
            // we must update the position when the table was reloaded
            const Point point(_cardSize.width * (i + 0.5f) - cardNodeOffset.x / 2, cardNode->getContentSize().height * 0.5f);
            cardNode->setPosition(point + Point(0, (idx == maxCnt - 1) ? cardNodeOffset.y : 0));
            
            const string& name = _candidateCards.at(index);
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
    return getCellsCount();
}

#pragma mark - CardNodeObserver
void FormationLayer::onCardNodeTouchedBegan(CardNode* node)
{
    _selectedCard = node->getCardName();
    const auto& point = convertToNodeSpace(node->getParent()->convertToWorldSpace(node->getPosition()));
    createDraggingNode(_selectedCard, point);
    if (_pickedCards.find(_selectedCard) != _pickedCards.end()) {
        _draggingNode->setVisible(true);
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
            auto node = Sprite::create("GameImages/public/test/f_1.png");
            addChild(node);
            
            const auto& size = node->getContentSize();
            node->setPosition(_tileBasePosition + Point(size.width * (i + 0.5f), -(size.height * (j + 0.5f))));
            
            TileInfo info;
            info.idx = j * FORMATION_WIDTH + i;
            info.node = node;
            info.midPoint = node->getPosition();
            _tiles.push_back(info);
            
            if (0 == _tileSize.width) {
                _tileSize = node->getContentSize();
            }
        }
    }
}

#pragma mark table
void FormationLayer::createTableView()
{
    TableView* tableView = TableView::create(this, _tableMaxSize);
    tableView->setDelegate(this);
    tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
    tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setBounceable(false);
    addChild(tableView);
    
    _table = tableView;
    
    refreshTable(false);
}

void FormationLayer::refreshTable(bool reload)
{
    if (_table) {
        auto totalHeight = _cardSize.height * getCellsCount() + cardNodeOffset.y;
        auto size = Size(_tableMaxSize.width, MIN(totalHeight, _tableMaxSize.height));
        _table->setViewSize(size);
        _table->setPosition(_tableBasePosition - Point(0, size.height));
        
        if (reload) {
            const Point& offset = _table->getContentOffset();
            _table->reloadData();
            _table->setContentOffset(offset);
        }
    }
}

ssize_t FormationLayer::getCellsCount() const
{
    const size_t cnt(_candidateCards.size());
    if (cnt > 0) {
        return (cnt - 1) / columnCount + 1;
    }
    
    return 0;
}

Size FormationLayer::getCellSize() const
{
    return Size(_tableMaxSize.width, _cardSize.height);
}

Rect FormationLayer::getBoundingBox(Node* node) const
{
    if (node) {
        cocos2d::Rect rect = node->getBoundingBox();
        Point origin = rect.origin;
        rect.origin = convertToNodeSpace(node->getParent()->convertToWorldSpace(origin));
        return rect;
    }
    
    return cocos2d::Rect::ZERO;
}

#pragma mark buttons
void FormationLayer::createExitButton()
{
    const Size& winSize = getWinSize();
    static const string file("GameImages/test/ui_guanbi.png");
    auto button = Button::create(file, file);
    button->addClickEventListener([this](Ref*) {
        removeFromParent();
    });
    addChild(button);
    
    static const Vec2 offset(5.0f, 5.0f);
    const Size& size = button->getContentSize();
    button->setPosition(Point(winSize.width - size.width / 2, winSize.height - size.height / 2) - offset);
}

void FormationLayer::createSwitchFormationButton()
{
    const auto basePosition = _tileBasePosition + Point(_tileSize.width * FORMATION_WIDTH, 0);
    for (int i = FORMATION_MAX_COUNT - 1; i >= 0; --i) {
        const int formationId = i + 1;
        const string file = StringUtils::format("GameImages/public/test/f_b_%d.png", formationId);
        auto button = Button::create(file, file);
        button->addClickEventListener([this, formationId](Ref*) {
            loadFormation(formationId);
        });
        addChild(button);
        
        static const Vec2 offset(10.0f, 5.0f);
        const Size& size = button->getContentSize();
        const int idx = FORMATION_MAX_COUNT - i - 1;
        button->setPosition(basePosition - Point((size.width + offset.x) * idx + size.width / 2, - (offset.y + size.height / 2)));
    }
}

void FormationLayer::createSaveFormationButton(const Point& position)
{
    static const string file("GameImages/public/test/f_b_save.png");
    auto button = Button::create(file, file);
    button->addClickEventListener([this](Ref*) {
        saveFormation();
    });
    addChild(button);
    
    const auto& size = button->getContentSize();
    button->setPosition(position + Point(size.width / 2, 0));
}

void FormationLayer::createSetDefaultFormationButton(const Point& position)
{
    static const string file("GameImages/public/test/f_b_default.png");
    auto button = Button::create(file, file);
    button->addClickEventListener([this](Ref*) {
        setDefaultFormation();
    });
    addChild(button);
    
    const auto& size = button->getContentSize();
    button->setPosition(position - Point(size.width / 2, 0));
}

#pragma mark labels
void FormationLayer::updatePopulationCount(int count)
{
    const string msg = StringUtils::format("%d/%d", count, FORMATION_POPULATION_COUNT);
    if (!_populationLabel) {
        auto hintLabel = CocosUtils::createLabel("人口资源", DEFAULT_FONT_SIZE);
        addChild(hintLabel);
        
        _populationLabel = CocosUtils::createLabel(msg, DEFAULT_FONT_SIZE);
        addChild(_populationLabel);
        
        // set positions
        const auto& hintSize = hintLabel->getContentSize();
        {
            static const Point offset(0, 5.0f);
            hintLabel->setPosition(_tileBasePosition + Point(hintSize.width / 2, hintSize.height / 2) + offset);
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

void FormationLayer::updateSpellsCount(int count)
{
    const string msg = StringUtils::format("%d/%d", count, FORMATION_POPULATION_COUNT);
    if (!_spellCountLabel) {
        auto hintLabel = CocosUtils::createLabel("人口资源", DEFAULT_FONT_SIZE);
        addChild(hintLabel);
        
        _spellCountLabel = CocosUtils::createLabel(msg, DEFAULT_FONT_SIZE);
        addChild(_spellCountLabel);
        
        // set positions
        const auto& hintSize = hintLabel->getContentSize();
        {
            static const Point offset(0, 5.0f);
            hintLabel->setPosition(_tileBasePosition + Point(hintSize.width / 2, hintSize.height / 2) + offset);
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
        int rarity(0);
        int cost(0);
        if (_techTree) {
            auto ut = _techTree->findUnitTypeByName(name);
            if (ut) {
                rarity = ut->getRarity();
            }
            
            auto ct = _gameModeHMM->findCardTypeByName(name);
            if (ct) {
                const auto& costs = ct->getCost();
                static const string& name(RESOURCE_NAME);
                if (costs.find(name) != costs.end()) {
                    cost = costs.at(name) / GameConstants::MICRORES_PER_RES;
                }
            }
        }
        
        node->update(name, rarity, cost, BATTLE_RESOURCE_MAX_COUNT);
    }
}

void FormationLayer::createDraggingNode(const string& name, const Point& point)
{
    if (!_draggingNode && name.length() > 0) {
        auto node = Sprite::create("GameImages/effects/backcircle_bule.png");
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
    auto node = FormationUnitNode::create(name, _tileSize);
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

#pragma mark functions
void FormationLayer::saveFormation()
{
    
}

void FormationLayer::loadFormation(int idx)
{
    
}

void FormationLayer::setDefaultFormation()
{
    
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
        
        // save info
        FormationInfo infoA;
        infoA.node = nodeA;
        infoA.cardName = nodeA->getUnitName();
        
        // if the position is occupied
        if (_formation.find(idxB) != end(_formation)) {
            _formation.at(idxB).node->removeFromParent();
            _formation.at(idxB) = infoA;
        } else {
            _formation.insert(make_pair(idxB, infoA));
        }
        
        // place
        placeUnit(nodeA, tileInfoB.midPoint);
    }
}

void FormationLayer::onUnitTouchedBegan(FormationUnitNode* node)
{
    if (node) {
        const auto& point = node->getTouchBeganPosition();
        createDraggingNode(node->getUnitName(), point);
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
        if (idxA >= 0 && _formation.find(idxA) != end(_formation)) {
            const auto& tileInfoB = getTileInfo(nodeA->getTouchEndPosition());
            int idxB(tileInfoB.idx);
            if (idxB >= 0) {
                const auto& pointB = tileInfoB.midPoint;
                // if the position is occupied
                if (_formation.find(idxB) != end(_formation)) {
                    const auto& infoB(_formation.at(idxB));
                    auto nodeB = infoB.node;
                    
                    // exchange
                    if (nodeB != nodeA) {
                        const auto& pointA = tileInfoA.midPoint;
                        placeUnit(nodeA, pointB);
                        placeUnit(nodeB, pointA);
                        
                        const auto& infoA(_formation.at(idxA));
                        _formation.at(idxA) = infoB;
                        _formation.at(idxB) = infoA;
                    }
                } else {
                    placeUnit(nodeA, pointB);
                    
                    const auto& infoA(_formation.at(idxA));
                    _formation.insert(make_pair(idxB, infoA));
                    _formation.erase(idxA);
                }
            } else {
                nodeA->removeFromParent();
                _formation.erase(idxA);
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
    static const int zorder_max(zorder_min + FORMATION_WIDTH * FORMATION_HEIGHT);
    
    return fmax(zorder_max - point.y, zorder_min);
}
