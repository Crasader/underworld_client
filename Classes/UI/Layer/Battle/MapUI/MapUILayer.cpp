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
#include "ResourceButton.h"
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
,_createdTableViewsCount(0)
,_tableViewPos(Point::ZERO)
,_isTouchingTableView(false)
,_selectedCamp(nullptr)
,_timeLabel(nullptr)
,_nextWaveTimeLabel(nullptr)
,_nextWaveProgress(nullptr)
,_goldResourceButton(nullptr)
,_woodResourceButton(nullptr)
,_populationLabel(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_pauseMenuItem(nullptr)
{
    static const Size unitNodeSize = MapUIUnitNode::create(nullptr)->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY * 2;
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
    if (0 == _createdTableViewsCount) {
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

void MapUILayer::updateWaveTime(int time, int totalTime)
{
    if (_nextWaveTimeLabel) {
        _nextWaveTimeLabel->setString(StringUtils::format("%ds", time));
    }
    
    if (_nextWaveProgress) {
        _nextWaveProgress->setPercentage(100.0f * time / totalTime);
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

void MapUILayer::updatePopulation(int count, int maxCount)
{
    if (_populationLabel) {
        _populationLabel->setString(StringUtils::format("%d/%d", count, maxCount));
    }
}

void MapUILayer::updateGoldAndWood(int gold, int wood)
{
    if (_goldResourceButton) {
        _goldResourceButton->setCount(gold);
    }
    
    if (_woodResourceButton) {
        _woodResourceButton->setCount(wood);
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
            const int goldCount = _goldResourceButton->getCount();
            const int woodCount = _woodResourceButton->getCount();
            if (unitNode) {
                unitNode->reuse(camp, idx, goldCount, woodCount);
                unitNode->setSelected(table == _selectedCampIdx.first && idx == _selectedCampIdx.second);
                unitNode->setTouched(table == _touchedCampIdx.first && idx == _touchedCampIdx.second, isGameOver());
            } else {
                unitNode = MapUIUnitNode::create(camp);
                unitNode->reuse(camp, idx, goldCount, woodCount);
                unitNode->setPosition(Point(unitNodeOffsetX, unitNodeOffsetY));
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
void MapUILayer::onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node)
{
    _selectedCamp = node->getCamp();
    
    clearTouchedCampIdx();
    UnitClass uc = node->getCamp()->getUnitType()->getUnitClass();
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
        static const float ceilOffset(10.0f);
        static const Size progressSize(170.0f, 20.0f);
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_xt.png");
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2 - 192.0f, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
            Label* label = CocosUtils::createLabel(myAccount, DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _myHpProgress = createProgressTimer("GameImages/test/ui_xt_1.png");
            _myHpProgress->setPosition(Point(size.width / 2, 8.0f));
            sprite->addChild(_myHpProgress);
            
            _myHpPercentageLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _myHpPercentageLabel->setPosition(_myHpProgress->getPosition());
            sprite->addChild(_myHpPercentageLabel);
        }
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/icon_zg.png");
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2 - 88.0f, winSize.height - ceilOffset));
            root->addChild(sprite);
        }
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_sj.png");
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
            Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_mapUI_battleTime"), DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _timeLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _timeLabel->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_timeLabel);
        }
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/icon_bc.png");
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2 + 88.0f, winSize.height - ceilOffset));
            root->addChild(sprite);
        }
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_xt.png");
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2 + 192.0f, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
            Label* label = CocosUtils::createLabel(opponentsAccount, DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _opponentsHpProgress = createProgressTimer("GameImages/test/ui_xt_1.png");
            _opponentsHpProgress->setPosition(Point(size.width / 2, 10.0f));
            sprite->addChild(_opponentsHpProgress);
            
            _opponentsHpPercentageLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _opponentsHpPercentageLabel->setPosition(_opponentsHpProgress->getPosition());
            sprite->addChild(_opponentsHpPercentageLabel);
        }
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_black_12.png");
            sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            sprite->setPosition(Point(leftOffset, ceilOffset));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
//            Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_mapUI_totalPopulation"), DEFAULT_FONT_SIZE);
//            label->setPosition(Point(size.width / 2, size.height * 0.875));
//            sprite->addChild(label);
            
            _populationLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _populationLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            _populationLabel->setPosition(Point(80, 94));
            sprite->addChild(_populationLabel);
            
//            label = CocosUtils::createLabel(LocalHelper::getString("battle_mapUI_totalResource"), DEFAULT_FONT_SIZE);
//            label->setPosition(Point(size.width / 2, size.height * 0.375));
//            sprite->addChild(label);
            
            _woodResourceButton = ResourceButton::create(true, true, false, kResourceType_Wood, 0, WOOD_LABEL_COLOR, nullptr);
            _woodResourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _woodResourceButton->setPosition(Point(size.width / 2, 60));
            sprite->addChild(_woodResourceButton);
            
            _goldResourceButton = ResourceButton::create(true, true, false, kResourceType_Gold, 0, GOLD_LABEL_COLOR, nullptr);
            _goldResourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _goldResourceButton->setPosition(Point(size.width / 2, 20));
            sprite->addChild(_goldResourceButton);
            
            // units table
            _tableViewPos = sprite->getPosition() + Point(sprite->getContentSize().width + leftOffset, - unitNodeOffsetY);
            _tableViewMaxSize.width = winSize.width - (_tableViewPos.x + leftOffset);
            _tableViewMaxSize.height = _cellSize.height;
        }
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_black_14.png");
            sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            static const float offsetY(2.0f);
            sprite->setPosition(_tableViewPos + Point(0, _cellSize.height + offsetY));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
            Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_mapUI_nextWaveTime"), 16);
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            label->setPosition(Point(20, size.height * 0.7f));
            sprite->addChild(label, 1);
            
            _nextWaveTimeLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _nextWaveTimeLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            _nextWaveTimeLabel->setPosition(Point(size.width * 0.75f, label->getPosition().y));
            sprite->addChild(_nextWaveTimeLabel, 1);
            
            Sprite* progressBg = Sprite::create("GameImages/test/ui_blood_7.png");
            progressBg->setPosition(Point(size.width / 2, size.height * 0.4f));
            sprite->addChild(progressBg);
            
            _nextWaveProgress = createProgressTimer("GameImages/test/ui_blood_6.png");
            _nextWaveProgress->setPosition(progressBg->getPosition());
            sprite->addChild(_nextWaveProgress);
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
bool MapUILayer::isGameOver() const
{
    if (_observer) {
        return _observer->onMapUILayerIsGameOver();
    }
    
    return false;
}

void MapUILayer::onUnitTouched(MapUIUnitNode* node)
{
    UnitClass uc = node->getCamp()->getUnitType()->getUnitClass();
    TableView* table = _tableViews.at(uc);
    const ssize_t idx = node->getIdx();
    
    TableView* lastTable = _selectedCampIdx.first;
    ssize_t lastIdx = _selectedCampIdx.second;
    
    _selectedCampIdx.first = table;
    _selectedCampIdx.second = idx;
    
    if (table != lastTable) {
        if (lastTable != nullptr && lastIdx != CC_INVALID_INDEX) {
            lastTable->updateCellAtIndex(lastIdx);
        }
        
        table->updateCellAtIndex(idx);
    } else {
        if (lastIdx != idx) {
            if (lastIdx != CC_INVALID_INDEX) {
                table->updateCellAtIndex(lastIdx);
            }
            
            table->updateCellAtIndex(idx);
        }
    }
    
    SoundManager::getInstance()->playButtonSelectUnitSound();
    
    if (_observer) {
        _observer->onMapUILayerUnitSelected(node->getCamp());
    }
}

void MapUILayer::clearTouchedCampIdx()
{
    _touchedCampIdx.first = nullptr;
    _touchedCampIdx.second = CC_INVALID_INDEX;
}

void MapUILayer::createTableViews()
{
    static float offsetX(10.0f);
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
    ssize_t cnt(0);
    if (_observer) {
        cnt = _observer->onMapUILayerCampsCount(uc);
    }
    if (cnt > 0) {
        const int width = _cellSize.width * cnt + unitNodeOffsetX;
        const Size size(width, _cellSize.height);
        
        static const float offset(5.0f);
        Rect rect(0, 0, 54, 114);
        static const float capInsets(4.0f);
        Rect capInsetsRect(capInsets, capInsets, rect.size.width - capInsets, rect.size.height - capInsets);
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
        ++ _createdTableViewsCount;
        return sprite;
    }
    
    return nullptr;
}

UnitClass MapUILayer::getUnitClass(TableView* table) const
{
    if (_createdTableViewsCount < tablesCount) {
        return tableUnitClass[_createdTableViewsCount];
    }
    
    return static_cast<UnitClass>(table->getTag());
}

ssize_t MapUILayer::getCellsCount(TableView* table) const
{
    if (_observer) {
        return _observer->onMapUILayerCampsCount(getUnitClass(table));
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
