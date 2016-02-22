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
#include "SoundManager.h"

using namespace std;
using namespace ui;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const float unitNodeOffsetX(5.0f);
static const float unitNodeOffsetY(0.0f);

static const UnitClass tableUnitClass[] = {
    kUnitClass_Hero,
    kUnitClass_Warrior,
};
static const size_t tablesCount = sizeof(tableUnitClass) / sizeof(UnitClass);

// test data
static const string heroes[] = {
    "时光女神",
    "狼人森林之魂",
    "天空女神",
    "蛇女莉莉丝",
    "时光法师",
};
static const size_t heroesCount = sizeof(heroes) / sizeof(string);

static const string warriors[] = {
    "狼人步兵",
    "狼人机械狼",
    "狼人射手",
    "机械甲虫",
    "狼人巫师",
    "吸血鬼战士",
    "吸血鬼弓箭手",
    "巨型蜘蛛",
    "七恶魔拉玛什图",
    "甲壳虫",
    "吸血鬼巫师",
};
static const size_t warriorsCount = sizeof(warriors) / sizeof(string);

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
{
    static const Size unitNodeSize = BattleDeckTestNode::create("", false)->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY * 2 + 6.0f;
    _cellSize.width = unitNodeSize.width + unitNodeOffsetX;
}

BattleDeckLayer::~BattleDeckLayer()
{
    removeAllChildren();
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
        _infoNode = BattleDeckUnitInfoNode::create(nullptr);
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
        static const unsigned int cardsCount = 8;
        Point firstCardPos(Point::ZERO);
        for (int i = 0; i < cardsCount; ++i) {
            Sprite* sprite = Sprite::create("GameImages/test/ui_jiabing.png");
            const Size& size = sprite->getContentSize();
            if (firstCardPos.x == 0) {
                firstCardPos.x = marginX + size.width / 2;
                firstCardPos.y = marginY + size.height / 2;
            }
            sprite->setPosition(firstCardPos + Point((size.width + marginX) * i, 0));
            parent->addChild(sprite);
            _cardDecks.push_back(sprite);
        }
        
        // ready button
        static const string file("GameImages/test/button_yellow_5.png");
        Button* button = Button::create(file, file);
        parent->addChild(button);
        
        const Size& buttonSize = button->getContentSize();
        button->setPosition(Point(bgSize.width - (buttonSize.width / 2 + marginX), buttonSize.height / 2 + marginY));
                
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
    
}

void BattleDeckLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDelegate
void BattleDeckLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    
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
    
    string name;
    UnitClass uc = getUnitClass(table);
    const bool isHero(kUnitClass_Hero == uc);
    if (isHero) {
        name = heroes[idx];
    } else {
        name = warriors[idx];
    }
    
    const Camp* camp = nullptr;
    if (name.length() > 0) {
        static const int nodeTag(100);
        BattleDeckTestNode* unitNode = dynamic_cast<BattleDeckTestNode*>(cell->getChildByTag(nodeTag));
        if (unitNode) {
            unitNode->reuse(name, idx);
            unitNode->setSelected(table == _selectedCampIdx.first && idx == _selectedCampIdx.second);
        } else {
            unitNode = BattleDeckTestNode::create(name, isHero);
            unitNode->reuse(name, idx);
            const Size& size = unitNode->getContentSize();
            unitNode->setPosition(Point(size.width / 2, size.height / 2) + Point(unitNodeOffsetX, unitNodeOffsetY));
            unitNode->registerObserver(this);
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
void BattleDeckLayer::onBattleDeckUnitNodeTouchedBegan(const Camp* camp)
{
    
}

void BattleDeckLayer::onBattleDeckUnitNodeTouchedEnded(const Camp* camp, bool isValid)
{
    
}

void BattleDeckLayer::onBattleDeckUnitNodeTouchedCanceled(const Camp* camp)
{
    
}

#pragma mark - BattleDeckTestNodeObserver
void BattleDeckLayer::onBattleDeckTestNodeTouchedBegan(const std::string& name)
{
    
}

void BattleDeckLayer::onBattleDeckTestNodeTouchedEnded(const std::string& name, bool isValid)
{
    
}

void BattleDeckLayer::onBattleDeckTestNodeTouchedCanceled(const std::string& name)
{
    
}

#pragma mark - protected
void BattleDeckLayer::createTableViews(float width)
{
    static float offsetY(5.0f);
    vector<Node*> created;
    for (int i = 0; i < tablesCount; ++i) {
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
}

Node* BattleDeckLayer::createTableView(UnitClass uc, float width)
{
    const ssize_t cnt = getCellsCount(uc);
    if (cnt > 0) {
        Node* parent = Node::create();
        
        // title
        string message;
        if (kUnitClass_Hero == uc) {
            message = "Heroes";
        } else if (kUnitClass_Warrior == uc) {
            message = "Warriors";
        }
        
        Sprite* title = CocosUtils::createTitle(message + StringUtils::format(":%ld", cnt), BIG_FONT_SIZE);
        parent->addChild(title);
        
        // table view
        static const float offsetX(5.0f);
        static const float offsetY(5.0f);
        
        static const string leftFile("GameImages/public/button_sanjiao_3.png");
        Button* leftButton = Button::create(leftFile, leftFile);
        parent->addChild(leftButton);
        
        static const string rightFile("GameImages/public/button_sanjiao_2.png");
        Button* rightButton = Button::create(rightFile, rightFile);
        parent->addChild(rightButton);
        
        const float leftButtonWidth = leftButton->getContentSize().width;
        const float rightButtonWidth = rightButton->getContentSize().width;
        const float maxWidth = width - (leftButtonWidth + rightButtonWidth + offsetX * 2);
        const float totalWidth = _cellSize.width * cnt + unitNodeOffsetX;
        const Size tableSize(MIN(maxWidth, totalWidth), _cellSize.height);
        
        TableView* tableView = TableView::create(this, tableSize);
        tableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
        tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        tableView->setBounceable(false);
        tableView->setTag(uc);
        parent->addChild(tableView);
        _tableViews.insert(make_pair(uc, tableView));
        
        const bool canScroll(maxWidth < totalWidth);
        leftButton->setVisible(canScroll);
        rightButton->setVisible(canScroll);
        leftButton->addClickEventListener([](Ref*) {
            
        });
        rightButton->addClickEventListener([](Ref*) {
            
        });
        
        // set positions
        const Size& titleSize = title->getContentSize();
        const Size size(width, titleSize.height + offsetY + tableSize.height);
        parent->setContentSize(size);
        
        title->setPosition(Point(size.width / 2, size.height - titleSize.height / 2));
        leftButton->setPosition(Point(leftButtonWidth / 2, tableSize.height / 2));
        rightButton->setPosition(Point(size.width - rightButtonWidth / 2, leftButton->getPositionY()));
        tableView->setPosition(Point(leftButtonWidth + offsetX, 0));
        
        return parent;
    }
    
    return nullptr;
}

UnitClass BattleDeckLayer::getUnitClass(TableView* table) const
{
    const size_t createdCount = _tableViews.size();
    if (createdCount < tablesCount) {
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
    if (kUnitClass_Hero == uc) {
        return heroesCount;
    } else if (kUnitClass_Warrior == uc) {
        return warriorsCount;
    }
    
    return 0;
}

Rect BattleDeckLayer::getTableViewBoundingBox(TableView* table) const
{
    Rect rect = table->getBoundingBox();
    Point origin = rect.origin;
    rect.origin = convertToNodeSpace(table->getParent()->convertToWorldSpace(origin));
    return rect;
}
