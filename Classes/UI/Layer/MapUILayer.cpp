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
#include "Game.h"
#include "UnitType.h"
#include "TechTree.h"
#include "ResourceButton.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int waveTime = 20;
static const int battleTotalTime = 180;
static const float unitNodeOffsetX = 5.0f;
static const float unitNodeOffsetY = 2.0f;
static const int visibleCellsCount = 8;

static ProgressTimer* createProgressTimer()
{
    Sprite* s = Sprite::create("GameImages/test/ui_xt_1.png");
    ProgressTimer* pt = ProgressTimer::create(s);
    pt->setType(ProgressTimer::Type::BAR);
    pt->setBarChangeRate(Vec2(1.0f, 0.0f));
    pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
    
    return pt;
}

#pragma mark =====================================================
#pragma mark Unit Node
#pragma mark =====================================================

MapUIUnitNode* MapUIUnitNode::create(const UnitType* type, ssize_t idx)
{
    MapUIUnitNode *ret = new (nothrow) MapUIUnitNode();
    if (ret && ret->init(type, idx))
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

MapUIUnitNode::MapUIUnitNode()
:_observer(nullptr)
,_iconButton(nullptr)
,_resourceButton(nullptr)
,_countLabel(nullptr)
,_unitType(nullptr)
,_idx(CC_INVALID_INDEX)
,_touchInvalid(false)
{
    
}

MapUIUnitNode::~MapUIUnitNode()
{
    removeAllChildren();
}

bool MapUIUnitNode::init(const UnitType* type, ssize_t idx)
{
    if (Node::init())
    {
#if false
        const string CsbFile = "facebook_UI.csb";
        Node *mainNode = CSLoader::createNode(CsbFile);
        addChild(mainNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        Widget* root = dynamic_cast<Widget *>(mainNode->getChildByTag(501));
        root->setSwallowTouches(false);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                const Point& position = child->getPosition();
                Node *parent = child->getParent();
                switch (tag) {
                        
                }
            }
        }
#else
        static const string defaultIconFile("GameImages/test/icon_jz.png");
        _iconButton = Button::create(defaultIconFile, defaultIconFile);
        _iconButton->setPressedActionEnabled(false);
        _iconButton->setSwallowTouches(false);
        addChild(_iconButton);
        _iconButton->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
            } else if (type == Widget::TouchEventType::MOVED) {
                if (!_touchInvalid && button->getTouchMovePosition().distance(button->getTouchBeganPosition()) > TOUCH_CANCEL_BY_MOVING_DISTANCE) {
                    _touchInvalid = true;
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                if (!_touchInvalid) {
                    SoundManager::getInstance()->playButtonSound();
                    if(_observer) {
                        _observer->onMapUIUnitNodeTouchedEnded(this);
                    }
                }
            }
        });
        
        _resourceButton = ResourceButton::create(false, kResourceType_Gold, 100, nullptr);
        addChild(_resourceButton);
        
        _countLabel = CocosUtils::createLabel(StringUtils::format("X %d", 100), DEFAULT_FONT_SIZE);
        addChild(_countLabel);
        
        const float iconHeight = _iconButton->getContentSize().height;
        const float resourceHeight = _resourceButton->getContentSize().height;
        const float countHeight = _countLabel->getContentSize().height;
        static const float offsetY = 2.0f;
        const Size size(_iconButton->getContentSize().width, iconHeight + resourceHeight + countHeight + offsetY * 4);
        setContentSize(size);
        
        const float x = size.width / 2;
        const float y = size.height / 2;
        _iconButton->setPosition(Point(x, y));
        _resourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _resourceButton->setPosition(Point(x, y - (iconHeight + resourceHeight) / 2 - offsetY));
        _countLabel->setPosition(Point(x, y + (iconHeight + countHeight) / 2 + offsetY));
        
        update(type, idx);
#endif
        
        return true;
    }
    
    return false;
}

void MapUIUnitNode::registerObserver(MapUIUnitNodeObserver *observer)
{
    _observer = observer;
}

void MapUIUnitNode::update(const UnitType* type, ssize_t idx)
{
    _unitType = type;
    _idx = idx;
    
    const string iconFile = (type == nullptr) ? "GameImages/test/icon_jz.png" : "GameImages/test/icon_gjs.png";
    _iconButton->loadTextures(iconFile, iconFile);
}

void MapUIUnitNode::setSelected(bool selected)
{
    if (_iconButton) {
        Node *parent = _iconButton->getParent();
        if (parent) {
            static const int selectedTag = 100;
            Node *selectedSprite = parent->getChildByTag(selectedTag);
            if (selected) {
                if (!selectedSprite) {
                    Sprite *s = Sprite::create("GameImages/test/ui_xz.png");
                    s->setTag(selectedTag);
                    s->setPosition(_iconButton->getPosition());
                    parent->addChild(s, 1);
                }
            } else {
                if (selectedSprite) {
                    parent->removeChild(selectedSprite);
                }
            }
        }
    }
}

const UnitType* MapUIUnitNode::getUnitType() const
{
    return _unitType;
}

ssize_t MapUIUnitNode::getIdx() const
{
    return _idx;
}

#pragma mark =====================================================
#pragma mark MapUIUnitCell
#pragma mark =====================================================

MapUIUnitNode* MapUIUnitCell::getUnitNode() const
{
    return _unitNode;
}

void MapUIUnitCell::setUnitNode(MapUIUnitNode* node)
{
    assert(!_unitNode);
    _unitNode = node;
}

void MapUIUnitCell::resetUnitNode()
{
    _unitNode = nullptr;
}

#pragma mark =====================================================
#pragma mark MapUILayer
#pragma mark =====================================================

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
,_paused(false)
,_cellsCount(16)
,_selectedUnitIdx(CC_INVALID_INDEX)
,_tableView(nullptr)
,_timeLabel(nullptr)
,_nextWaveTimeLabel(nullptr)
,_energyResourceButton(nullptr)
,_populationLabel(nullptr)
,_unitCostLabel(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_sendTroopMenuItem(nullptr)
,_pauseMenuItem(nullptr)
,_waveTime(waveTime)
,_remainingTime(battleTotalTime)
{
    static const Size& unitNodeSize = MapUIUnitNode::create(nullptr, 0)->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY * 2;
    _cellSize.width = unitNodeSize.width + unitNodeOffsetX * 2;
    
    _tableViewMaxSize.width = _cellSize.width * visibleCellsCount;
    _tableViewMaxSize.height = _cellSize.height;
}

MapUILayer::~MapUILayer()
{
    removeAllChildren();
}

void MapUILayer::registerObserver(MapUILayerObserver *observer)
{
    _observer = observer;
}

void MapUILayer::initWithGame(const Game* game)
{
    if (game) {
        const World* world = game->getWorld();
        if (world) {
            const TechTree* techTree = world->getTechTree();
            const Faction* faction = world->getFaction(0);
            int count = techTree->getResourceTypeCount();
            if (count > 0)
            {
                const UnderWorld::Core::ResourceType* resourceType = techTree->getResourceTypeByIndex(0);
                if (resourceType->_class == kResourceClass_holdable) {
                    const Resource* resource = faction->getResource(resourceType);
                    _populationLabel->setString(StringUtils::format("%d/%d", resource->getOccpied(), resource->getBalance()));
                } else {
                    
                }
            }
            else if (count > 1)
            {
                
            }
            
            // reload table view
            reloadTableView(world->getCampCount(world->getThisFactionIndex()));
        }
    }
}

void MapUILayer::updateMyHpProgress(int progress)
{
    _myHpProgress->setPercentage(progress);
    _myHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
}

void MapUILayer::updateOpponentsHpProgress(int progress)
{
    _opponentsHpProgress->setPercentage(progress);
    _opponentsHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
}

void MapUILayer::updateWaveTime(int time)
{
    _nextWaveTimeLabel->setString(StringUtils::format("%ds", time));
}

void MapUILayer::updateRemainingTime(int time)
{
    _timeLabel->setString(CocosUtils::getFormattedTime(time));
}

#pragma mark - TableViewDelegate
void MapUILayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    onUnitTouched(cell->getIdx());
}

#pragma mark - TableViewDataSource
Size MapUILayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return _cellSize;
}

TableViewCell* MapUILayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    MapUIUnitCell *cell = static_cast<MapUIUnitCell*>(table->dequeueCell());
    if (!cell)
    {
        cell = MapUIUnitCell::create();
        
        // TODO: remove test code
        UnitType* type = (idx % 2 == 0) ? nullptr : reinterpret_cast<UnitType*>(1);
        MapUIUnitNode* unitNode = MapUIUnitNode::create(type, idx);
        unitNode->setPosition(Point(unitNodeOffsetX, unitNodeOffsetY));
        unitNode->registerObserver(this);
        cell->addChild(unitNode);
        cell->setUnitNode(unitNode);
    }
    else
    {
        // TODO: remove test code
        UnitType* type = (idx % 2 == 0) ? nullptr : reinterpret_cast<UnitType*>(1);
        MapUIUnitNode* unitNode = cell->getUnitNode();
        unitNode->update(type, idx);
        unitNode->setSelected(idx == _selectedUnitIdx);
    }
    
    return cell;
}

ssize_t MapUILayer::numberOfCellsInTableView(TableView *table)
{
    return _cellsCount;
}

#pragma mark - MapUIUnitNodeObserver
void MapUILayer::onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node)
{
    
}

#pragma mark -
bool MapUILayer::init(const string& myAccount, const string& opponentsAccount)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
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
        static const float leftOffset = 20.0f;
        static const float ceilOffset = 10.0f;
        static const Size progressSize(170.0f, 20.0f);
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/icon_nl.png");
            sprite->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            sprite->setPosition(Point(leftOffset, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
            Label* label = CocosUtils::createLabel("下一波进攻时间", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _nextWaveTimeLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _nextWaveTimeLabel->setTextColor(Color4B::RED);
            _nextWaveTimeLabel->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_nextWaveTimeLabel);
        }
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
            
            _myHpProgress = createProgressTimer();
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
            
            Label* label = CocosUtils::createLabel("战斗时间", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _timeLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _timeLabel->setTextColor(Color4B::RED);
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
            
            _opponentsHpProgress = createProgressTimer();
            _opponentsHpProgress->setPosition(Point(size.width / 2, 10.0f));
            sprite->addChild(_opponentsHpProgress);
            
            _opponentsHpPercentageLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
            _opponentsHpPercentageLabel->setPosition(_opponentsHpProgress->getPosition());
            sprite->addChild(_opponentsHpPercentageLabel);
        }
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/icon_bg.png");
            sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            sprite->setPosition(Point(leftOffset, ceilOffset));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
            Label* label = CocosUtils::createLabel("训练后的总兵力", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.875));
            sprite->addChild(label);
            
            _populationLabel = CocosUtils::createLabel("0/100", DEFAULT_FONT_SIZE);
            _populationLabel->setPosition(Point(size.width / 2, size.height * 0.625));
            sprite->addChild(_populationLabel);
            
            label = CocosUtils::createLabel("能量", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.375));
            sprite->addChild(label);
            
            _energyResourceButton = ResourceButton::create(false, kResourceType_Gold, 2000, nullptr);
            _energyResourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _energyResourceButton->setPosition(Point(size.width / 2, size.height * 0.125));
            sprite->addChild(_energyResourceButton);
        }
        // units table
        {
            Sprite* sprite = CocosUtils::createPureColorSprite(_tableViewMaxSize - Size(4, 4), Color4B::BLACK);
            const Size& spriteSize(sprite->getContentSize());
            sprite->setPosition(Point(winSize.width / 2, ceilOffset + spriteSize.height / 2));
            root->addChild(sprite);
            
            _tableView = TableView::create(this, _tableViewMaxSize);
            _tableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
            _tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
            _tableView->setPosition(Point(sprite->getPosition().x - _tableViewMaxSize.width / 2, sprite->getPosition().y - _tableViewMaxSize.height / 2));
            _tableView->setBounceable(false);
            _tableView->setDelegate(this);
            root->addChild(_tableView);
        }
        // buttons
        {
            static const Size size(180, 60);
            Sprite* sprite = CocosUtils::createPureColorSprite(size, Color4B::ORANGE);
            _unitCostLabel = CocosUtils::createLabel(StringUtils::format("%d", 100), DEFAULT_FONT_SIZE);
            _unitCostLabel->setPosition(Point(size.width / 2, size.height / 2));
            sprite->addChild(_unitCostLabel);
            _pauseMenuItem = MenuItemImage::create("GameImages/test/ui_zt.png", "GameImages/test/ui_zt.png", [this](Ref*) {
                _paused = !_paused;
                if (_observer) {
                    _observer->onMapUILayerClickedPauseButton(_paused);
                }
            });
            _pauseMenuItem->setAnchorPoint(Point(1.0f, 1.0f));
            _pauseMenuItem->setPosition(Point(winSize.width - 5.0f, winSize.height - 5.0f));
            Menu *menu = Menu::create(/*_sendTroopMenuItem, */_pauseMenuItem, nullptr);
            menu->setPosition(Point::ZERO);
            root->addChild(menu);
        }
#endif
        
        updateWaveTime(_waveTime);
        updateRemainingTime(_remainingTime);
        updateMyHpProgress(60);
        updateOpponentsHpProgress(20);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [=](Touch *touch, Event *unused_event) {
            const Point& p = touch->getLocation();
            if (_tableView->getBoundingBox().containsPoint(p)) {
                return true;
            }
            return false;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void MapUILayer::onEnter()
{
    LayerColor::onEnter();
    _scheduler->schedule(schedule_selector(MapUILayer::fakeTick), this, 1.0f, false);
}

void MapUILayer::onExit()
{
    _scheduler->unschedule(schedule_selector(MapUILayer::fakeTick), this);
    LayerColor::onExit();
}

#pragma mark private
void MapUILayer::onUnitTouched(ssize_t idx)
{
    const ssize_t oldIdx = _selectedUnitIdx;
    const ssize_t newIdx = idx;
    if (newIdx != oldIdx) {
        _selectedUnitIdx = newIdx;
        
        if (oldIdx != CC_INVALID_INDEX) {
            _tableView->updateCellAtIndex(oldIdx);
        }
        
        _tableView->updateCellAtIndex(newIdx);
    }
    
    if (_observer) {
        _observer->onMapUILayerUnitSelected(idx % 2);
    }
}

void MapUILayer::reloadTableView(ssize_t cellsCount)
{
    _cellsCount = cellsCount;
    if (_tableView) {
        // if setTouchEnabled to false, tableCellTouched() will never be called
//        _tableView->setTouchEnabled(_cellsCount > visibleCellsCount);
        _tableView->reloadData();
        
        // fit
        if (false == _tableView->isBounceable() && extension::ScrollView::Direction::HORIZONTAL == _tableView->getDirection()) {
            _tableView->getContainer()->setPosition(Point::ZERO);
            const int contentWidth = _cellSize.width * _cellsCount;
            if (contentWidth >= _tableViewMaxSize.width) {
                _tableView->setViewSize(_tableViewMaxSize);
            } else {
                _tableView->setViewSize(Size(contentWidth, _tableViewMaxSize.height));
            }
        }
    }
}

void MapUILayer::fakeTick(float dt)
{
    -- _waveTime;
    -- _remainingTime;
    
    if (_waveTime < 0) {
        _waveTime = waveTime;
    }
    
    if (_remainingTime < 0) {
        _remainingTime = battleTotalTime;
    }
    
    updateWaveTime(_waveTime);
    updateRemainingTime(_remainingTime);
}