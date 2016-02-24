//
//  MapUILayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUILayer.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "Camp.h"
#include "MapUIUnitCell.h"
#include "BattleResourceNode.h"
#include "CampInfoNode.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const float unitNodeOffsetX(5.0f);
static const float unitNodeOffsetY(0.0f);

static const UnitClass tableUnitClass[] = {
    kUnitClass_Warrior,
    kUnitClass_Hero,
};
static const size_t tablesCount = sizeof(tableUnitClass) / sizeof(UnitClass);

static ProgressTimer* createProgressTimer(const string& file)
{
    Sprite* s = Sprite::create(file);
    ProgressTimer* pt = ProgressTimer::create(s);
    pt->setType(ProgressTimer::Type::BAR);
    pt->setBarChangeRate(Vec2(1.0f, 0.0f));
    pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
    
    return pt;
}

MapUILayer* MapUILayer::create(const string& myAccount, const string& opponentsAccount)
{
    MapUILayer *ret = new (nothrow) MapUILayer();
    if (ret && ret->init(myAccount, opponentsAccount))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

MapUILayer::MapUILayer()
:_observer(nullptr)
,_tableViewPos(Point::ZERO)
,_isTouchingTableView(false)
,_selectedCamp(nullptr)
,_timeLabel(nullptr)
,_goldResourceNode(nullptr)
,_woodResourceNode(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_pauseMenuItem(nullptr)
{
    static const Size unitNodeSize = MapUIUnitNode::create(nullptr)->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY * 2 + 6.0f;
    _cellSize.width = unitNodeSize.width + unitNodeOffsetX;
    
    _selectedCampIdx.first = nullptr;
    _selectedCampIdx.second = CC_INVALID_INDEX;
    
    clearTouchedCampIdx();
}

MapUILayer::~MapUILayer()
{
    removeAllChildren();
}

void MapUILayer::registerObserver(MapUILayerObserver *observer)
{
    _observer = observer;
}

void MapUILayer::reload()
{
    const size_t cnt = _tableViews.size();
    if (0 == cnt) {
        createTableViews();
    } else {
        for (map<UnitClass, TableView*>::iterator iter = _tableViews.begin(); iter != _tableViews.end(); ++iter) {
            reloadTable(iter->first);
        }
    }
}

void MapUILayer::reloadTable(UnitClass uc)
{
    if (_tableViews.find(uc) != _tableViews.end()) {
        TableView* table = _tableViews.at(uc);
        if (table) {
            const ssize_t cnt = getCellsCount(table);
            
            // if setTouchEnabled to false, tableCellTouched() will never be called
            const int visibleCellsCount = (_tableViewMaxSize.width - unitNodeOffsetX) / _cellSize.width;
            table->setTouchEnabled(cnt > visibleCellsCount);
            table->reloadData();
            
            // fit
            if (false == table->isBounceable() && extension::ScrollView::Direction::HORIZONTAL == table->getDirection()) {
                table->getContainer()->setPosition(Point::ZERO);
                const int contentWidth = _cellSize.width * cnt + unitNodeOffsetX;
                if (contentWidth >= _tableViewMaxSize.width) {
                    table->setViewSize(_tableViewMaxSize);
                } else {
                    table->setViewSize(Size(contentWidth, _tableViewMaxSize.height));
                }
            }
        }
    }
}

#if ENABLE_CAMP_INFO
void MapUILayer::insertCampInfo(size_t idx, const vector<pair<const Camp*, const UnitBase*>>& units)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->insert(units);
    }
}

void MapUILayer::updateCampInfos(size_t idx)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->update();
    }
}
#else
void MapUILayer::insertUnitInfos(size_t idx, const vector<const UnitBase*>& units)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->insert(units);
    }
}

void MapUILayer::updateUnitInfos(size_t idx)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->update();
    }
}
#endif

void MapUILayer::closeAllUnitInfoNodes()
{
    for (int i = 0; i < _campInfoNodes.size(); ++i) {
        CampInfoNode* node = _campInfoNodes.at(i);
        node->closeUnitInfoNode();
    }
}

void MapUILayer::updateMyHpProgress(int progress)
{
    if (_myHpProgress) {
        _myHpProgress->setPercentage(progress);
    }
    
    if (_myHpPercentageLabel) {
        progress = MAX(0, MIN(progress, 100));
        _myHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
    }
}

void MapUILayer::updateOpponentsHpProgress(int progress)
{
    if (_opponentsHpProgress) {
        _opponentsHpProgress->setPercentage(progress);
    }
    
    if (_opponentsHpPercentageLabel) {
        progress = MAX(0, MIN(progress, 100));
        _opponentsHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
    }
}

void MapUILayer::updateRemainingTime(int time)
{
    if (_timeLabel) {
        _timeLabel->setString(CocosUtils::getFormattedTime(time));
        
        if (time <= 180) {
            _timeLabel->setTextColor(Color4B::RED);
        } else {
            _timeLabel->setTextColor(Color4B::WHITE);
        }
    }
}

void MapUILayer::updateGoldAndWood(int gold, float decimalGold, int wood, float decimalWood)
{
    if (_goldResourceNode) {
        _goldResourceNode->setCount(gold, false);
        _goldResourceNode->setSubCount(decimalGold);
    }
    
    if (_woodResourceNode) {
        _woodResourceNode->setCount(wood, false);
        _goldResourceNode->setSubCount(decimalWood);
    }
}

void MapUILayer::pauseGame()
{
    
}

void MapUILayer::resumeGame()
{
    
}

bool MapUILayer::isPointInTableView(const Point& point)
{
    for (map<UnitClass, TableView*>::iterator iter = _tableViews.begin(); iter != _tableViews.end(); ++iter) {
        UnitClass uc = iter->first;
        TableView* tableView = iter->second;
        if (tableView && getTableViewBoundingBox(uc).containsPoint(point)) {
            return true;
        }
    }
    
    return false;
}

#pragma mark - TableViewDataSource
Size MapUILayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    const ssize_t cnt = getCellsCount(table);
    if (idx == cnt - 1) {
        return Size(_cellSize.width + unitNodeOffsetX, _cellSize.height);
    }
    
    return _cellSize;
}

TableViewCell* MapUILayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    MapUIUnitCell *cell = static_cast<MapUIUnitCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = MapUIUnitCell::create();
    }
    
    if (_observer) {
        UnitClass uc = getUnitClass(table);
        const Camp* camp = _observer->onMapUILayerCampAtIndex(uc, idx);
        if (camp) {
            MapUIUnitNode* unitNode = cell->getUnitNode();
            const int goldCount = _goldResourceNode->getCount();
            const int woodCount = _woodResourceNode->getCount();
            if (unitNode) {
                unitNode->reuse(camp, idx, goldCount, woodCount);
                unitNode->setSelected(table == _selectedCampIdx.first && idx == _selectedCampIdx.second);
                unitNode->setTouched(table == _touchedCampIdx.first && idx == _touchedCampIdx.second, isGameOver());
            } else {
                unitNode = MapUIUnitNode::create(camp);
                unitNode->reuse(camp, idx, goldCount, woodCount);
                const Size& size = unitNode->getContentSize();
                unitNode->setPosition(Point(size.width / 2, size.height / 2) + Point(unitNodeOffsetX, unitNodeOffsetY));
                unitNode->registerObserver(this);
                cell->addChild(unitNode);
                cell->setUnitNode(unitNode);
            }
        }
    }
    
    return cell;
}

ssize_t MapUILayer::numberOfCellsInTableView(TableView *table)
{
    return getCellsCount(table);
}

#pragma mark - MapUIUnitNodeObserver
void MapUILayer::onMapUIUnitNodeClickedAddButton(MapUIUnitNode* node)
{
    if (_observer) {
        _observer->onMapUILayerUnitAdd(node->getCamp());
    }
}

void MapUILayer::onMapUIUnitNodeClickedUpgradeButton(MapUIUnitNode* node)
{
    if (_observer) {
        _observer->onMapUILayerUnitUpgrade(node->getCamp());
    }
}

void MapUILayer::onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node)
{
    _selectedCamp = node->getCamp();
    
    clearTouchedCampIdx();
    UnitClass uc = node->getCamp()->getCurrentUnitType()->getUnitClass();
    if (_tableViews.find(uc) != _tableViews.end()) {
        TableView* table = _tableViews.at(uc);
        if (table) {
            _touchedCampIdx.first = table;
            _touchedCampIdx.second = node->getIdx();
        }
    }
}

void MapUILayer::onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node, bool isValid)
{
    if (!isGameOver()) {
        if (isValid) {
            onUnitTouched(node);
        }
        
        clearTouchedCampIdx();
    }
}

void MapUILayer::onMapUIUnitNodeTouchedCanceled(MapUIUnitNode* node)
{
    clearTouchedCampIdx();
}

#pragma mark - CampInfoNodeObserver
void MapUILayer::onCampInfoNodeClickedIcon(CampInfoNode* pSender, const UnitBase* unit)
{
    for (int i = 0; i < _campInfoNodes.size(); ++i) {
        CampInfoNode* node = _campInfoNodes.at(i);
        if (pSender != node) {
            node->closeUnitInfoNode();
        }
    }
}

#pragma mark -
bool MapUILayer::init(const string& myAccount, const string& opponentsAccount)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
#if false
        // yezi
        {
            static const string file("particle/yezi/yezi.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 - 10));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 + 10));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0_0_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 + 20));
            addChild(effect);
        }
#endif
        
#if false
        static const string CsbFile("rankInfo_UI.csb");
        Node *mainNode = CSLoader::createNode(CsbFile);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        Widget* root = dynamic_cast<Widget *>(mainNode->getChildByTag(777));
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                            
                    }
                }
            }
        }
#else
        Node* root = this;
        static const float leftOffset(5.0f);
        static const float ceilOffset(5.0f);
        static const Size progressSize(170.0f, 20.0f);
        
        createUserInfo(true, myAccount);
        
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_time.png");
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            sprite->setPosition(Point(winSize.width / 2, winSize.height - size.height / 2 - ceilOffset));
            
            _timeLabel = CocosUtils::createLabel("", 24, DEFAULT_NUMBER_FONT);
            _timeLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            _timeLabel->setPosition(Point(size.width * 0.45f, size.height / 2));
            sprite->addChild(_timeLabel);
        }
        
        createUserInfo(false, opponentsAccount);
        
        //
        {
            _woodResourceNode = BattleResourceNode::create(kResourceType_Wood);
            root->addChild(_woodResourceNode);
            
            const Size& size = _woodResourceNode->getContentSize();
            _woodResourceNode->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _woodResourceNode->setPosition(Point(size.width / 2 + leftOffset, size.height / 2 + ceilOffset));
            
            _goldResourceNode = BattleResourceNode::create(kResourceType_Gold);
            root->addChild(_goldResourceNode);
            
            _goldResourceNode->setAnchorPoint(_woodResourceNode->getAnchorPoint());
            _goldResourceNode->setPosition(_woodResourceNode->getPosition() + Point(size.width + leftOffset, 0));
            
            // units table
            _tableViewPos = Point(_goldResourceNode->getPositionX() + size.width / 2 + leftOffset, - unitNodeOffsetY);
            _tableViewMaxSize.width = winSize.width - (_tableViewPos.x + leftOffset);
            _tableViewMaxSize.height = _cellSize.height;
        }
        
        // buttons
        {
            _pauseMenuItem = MenuItemImage::create("GameImages/test/ui_zt.png", "GameImages/test/ui_zt.png", [this](Ref*) {
                if (_observer) {
                    _observer->onMapUILayerClickedPauseButton();
                }
            });
            _pauseMenuItem->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
            _pauseMenuItem->setPosition(Point(winSize.width - 5.0f, winSize.height - 5.0f));
            Menu *menu = Menu::create(_pauseMenuItem, nullptr);
            menu->setPosition(Point::ZERO);
            root->addChild(menu);
        }
        
        {
            CampInfoNode* campInfoNode = CampInfoNode::create(false);
            campInfoNode->registerObserver(this);
            const Size& size = campInfoNode->getContentSize();
            static const float posY = 160.0f;
            campInfoNode->setPosition(Point(0, posY));
            root->addChild(campInfoNode);
            _campInfoNodes.push_back(campInfoNode);
            
            campInfoNode = CampInfoNode::create(true);
            campInfoNode->registerObserver(this);
            campInfoNode->setPosition(Point(winSize.width - size.width, posY));
            root->addChild(campInfoNode);
            _campInfoNodes.push_back(campInfoNode);
        }
#endif
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MapUILayer::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(MapUILayer::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MapUILayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool MapUILayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    const Point& point = touch->getLocation();
    if (isPointInTableView(point)) {
        _isTouchingTableView = true;
        return true;
    }
    
    for (int i = 0; i < _campInfoNodes.size(); ++i) {
        CampInfoNode* node = _campInfoNodes.at(i);
        if (node) {
            const Rect& bb = node->getIconsBoundingBox();
            Rect boundingBox(Rect::ZERO);
            boundingBox.origin = convertToNodeSpace(node->convertToWorldSpace(bb.origin));
            boundingBox.size = bb.size;
            if (boundingBox.containsPoint(point)) {
                return true;
            }
        }
    }
    
    return false;
}

void MapUILayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    if (_isTouchingTableView && _selectedCamp && _observer) {
        const Point& point = touch->getLocation();
        _observer->onMapUILayerTouchMoved(_selectedCamp, point);
    }
}

void MapUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_isTouchingTableView && _selectedCamp) {
        if (_observer) {
            const Point& point = touch->getLocation();
            const bool cancelled = isPointInTableView(point);
            
            if (cancelled) {
                _observer->onMapUILayerTouchCancelled(_selectedCamp);
            } else {
                _observer->onMapUILayerTouchEnded(_selectedCamp, point);
            }
        }
        
        _selectedCamp = nullptr;
        _isTouchingTableView = false;
    }
}

#pragma mark private
void MapUILayer::createUserInfo(bool left, const string& account)
{
    const Size& winSize = Director::getInstance()->getWinSize();
    
    Sprite* nameBg = CocosUtils::createPureColorSprite(Size(163, 26), LAYER_MASK_COLOR);
    addChild(nameBg);
    
    const Size& nameBgSize = nameBg->getContentSize();
    static const float offsetX(192.0f);
    nameBg->setPosition(Point(winSize.width / 2 + offsetX * (left ? -1 : 1), winSize.height - nameBgSize.height / 2));
    
    const Point& position = nameBg->getPosition();
    
    // name
    {
        Label* label = CocosUtils::createLabel(account, DEFAULT_FONT_SIZE);
        label->setPosition(Point(nameBgSize.width / 2, nameBgSize.height / 2));
        nameBg->addChild(label);
    }
    
    // progress
    {
        Sprite* sprite = Sprite::create(left ? "GameImages/test/ui_blood_left_bg.png" : "GameImages/test/ui_blood_right_bg.png");
        addChild(sprite);
        
        const Size& size = sprite->getContentSize();
        sprite->setPosition(position - Point(0, (nameBgSize.height + size.height) / 2));
        
        ProgressTimer* pt = createProgressTimer(left ? "GameImages/test/ui_blood_left.png" : "GameImages/test/ui_blood_right.png");
        pt->setPosition(Point(size.width / 2, size.height / 2));
        pt->setMidpoint(left ? Point::ANCHOR_BOTTOM_RIGHT : Point::ANCHOR_BOTTOM_LEFT);
        sprite->addChild(pt);
        
        Label* ptLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        ptLabel->setPosition(pt->getPosition());
        sprite->addChild(ptLabel);
        
        if (left) {
            _myHpProgress = pt;
            _myHpPercentageLabel = ptLabel;
        } else {
            _opponentsHpProgress = pt;
            _opponentsHpPercentageLabel = ptLabel;
        }
    }
    
    // icon
    {
        Sprite* sprite = Sprite::create(left ? "GameImages/test/icon_zg.png" : "GameImages/test/icon_bc.png");
        addChild(sprite);
        
        const Size& size = sprite->getContentSize();
        sprite->setPosition(Point(position.x + (nameBgSize.width / 2 + size.width / 2) * (left ? 1 : -1), winSize.height - size.height / 2));
    }
}

bool MapUILayer::isGameOver() const
{
    if (_observer) {
        return _observer->onMapUILayerIsGameOver();
    }
    
    return false;
}

void MapUILayer::onUnitTouched(MapUIUnitNode* node)
{
    UnitClass uc = node->getCamp()->getCurrentUnitType()->getUnitClass();
    TableView* table = _tableViews.at(uc);
    const ssize_t idx = node->getIdx();
    
    TableView* lastTable = _selectedCampIdx.first;
    ssize_t lastIdx = _selectedCampIdx.second;
    
    const bool isSelected = (table != lastTable || lastIdx != idx);
    
    if (isSelected) {
        _selectedCampIdx.first = table;
        _selectedCampIdx.second = idx;
        
        if (table != lastTable) {
            if (lastTable != nullptr && lastIdx != CC_INVALID_INDEX) {
                lastTable->updateCellAtIndex(lastIdx);
            }
            
            table->updateCellAtIndex(idx);
        } else {
            if (lastIdx != CC_INVALID_INDEX) {
                table->updateCellAtIndex(lastIdx);
            }
            
            table->updateCellAtIndex(idx);
        }
    } else {
        _selectedCampIdx.first = nullptr;
        _selectedCampIdx.second = CC_INVALID_INDEX;
        
        if (lastTable != nullptr && lastIdx != CC_INVALID_INDEX) {
            lastTable->updateCellAtIndex(lastIdx);
        }
    }
    
    CCLOG("_selectedCampIdx: %ld", _selectedCampIdx.second);
    
    SoundManager::getInstance()->playButtonSelectUnitSound();
    
    if (_observer) {
        _observer->onMapUILayerUnitSelected(isSelected ? node->getCamp() : nullptr);
    }
}

void MapUILayer::clearTouchedCampIdx()
{
    _touchedCampIdx.first = nullptr;
    _touchedCampIdx.second = CC_INVALID_INDEX;
}

void MapUILayer::createTableViews()
{
    static float offsetX(0.0f);
    vector<Node*> created;
    for (int i = 0; i < tablesCount; ++i) {
        Node* node = createTableView(tableUnitClass[i], this);
        if (node) {
            const size_t cnt = created.size();
            Point pos(_tableViewPos);
            if (cnt > 0) {
                Node* n = created.back();
                pos = n->getPosition() + Point(n->getContentSize().width + offsetX, 0);
            }
            node->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            node->setPosition(pos);
            created.push_back(node);
        }
    }
}

Node* MapUILayer::createTableView(UnitClass uc, Node* parent)
{
    ssize_t cnt = getCellsCount(uc);
    if (cnt > 0) {
        const int width = _cellSize.width * cnt + unitNodeOffsetX;
        const Size size(width, _cellSize.height);
        
        static const float offset(5.0f);
        Rect rect(0, 0, 91, 157);
        static const float capInsets(18.0f);
        Rect capInsetsRect(capInsets, capInsets, rect.size.width - capInsets * 2, rect.size.height - capInsets * 2);
        Scale9Sprite* sprite = Scale9Sprite::create("GameImages/test/ui_black_13.png", rect, capInsetsRect);
        sprite->setContentSize(size + Size(offset * 2, 0));
        parent->addChild(sprite);
        
        TableView* tableView = TableView::create(this, size);
        tableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
        tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
        tableView->setBounceable(false);
        tableView->setTag(uc);
        tableView->setPosition(Point(offset, 0));
        sprite->addChild(tableView);
        _tableViews.insert(make_pair(uc, tableView));
        return sprite;
    }
    
    return nullptr;
}

UnitClass MapUILayer::getUnitClass(TableView* table) const
{
    const size_t cnt = _tableViews.size();
    if (cnt < tablesCount) {
        return tableUnitClass[cnt];
    }
    
    return static_cast<UnitClass>(table->getTag());
}

ssize_t MapUILayer::getCellsCount(TableView* table) const
{
    return getCellsCount(getUnitClass(table));
}

ssize_t MapUILayer::getCellsCount(UnitClass uc) const
{
    if (_observer) {
        return _observer->onMapUILayerCampsCount(uc);
    }
    
    return 0;
}

Rect MapUILayer::getTableViewBoundingBox(UnitClass uc) const
{
    if (_tableViews.find(uc) != _tableViews.end()) {
        TableView* tv = _tableViews.at(uc);
        Rect rect = tv->getBoundingBox();
        Point origin = rect.origin;
        rect.origin = convertToNodeSpace(tv->getParent()->convertToWorldSpace(origin));
        return rect;
    }
    
    return Rect::ZERO;
}
