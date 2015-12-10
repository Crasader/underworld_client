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
#include "Camp.h"
#include "TechTree.h"
#include "ResourceButton.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const int waveTime(20);
static const int battleTotalTime(600);
static const float unitNodeOffsetX(17.0f);
static const float unitNodeOffsetY(17.0f);
static const int visibleCellsCount(6);

static ProgressTimer* createProgressTimer()
{
    Sprite* s = Sprite::create("GameImages/test/ui_xt_1.png");
    ProgressTimer* pt = ProgressTimer::create(s);
    pt->setType(ProgressTimer::Type::BAR);
    pt->setBarChangeRate(Vec2(1.0f, 0.0f));
    pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
    
    return pt;
}

static string getIconFile(const string& unitName)
{
    static const string prefix("GameImages/test/");
    static const string suffix(".png");
    string file;
    
    if (WOLF_SOLDIER == unitName) {
        file = "icon_w_langdun";
    } else if (WOLF_ARCHER == unitName) {
        file = "icon_w_langgongjian";
    } else if (WOLF_WIZARD == unitName) {
        file = "icon_w_langfashi";
    } else if (VAMPIRE_SOLDIER == unitName) {
        file = "icon_v_roudun";
    } else if (VAMPIRE_ARCHER == unitName) {
        file = "icon_v_nvgongjianshou";
    } else if (VAMPIRE_WIZARD == unitName) {
        file = "icon_v_nvwushi";
    }
    
    if (file.length() > 0) {
        return prefix + file + suffix;
    }
    
    return "";
}

#pragma mark =====================================================
#pragma mark Unit Node
#pragma mark =====================================================

MapUIUnitNode* MapUIUnitNode::create(const Camp* camp, ssize_t idx)
{
    MapUIUnitNode *ret = new (nothrow) MapUIUnitNode();
    if (ret && ret->init(camp, idx))
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
,_camp(nullptr)
,_idx(CC_INVALID_INDEX)
,_touchInvalid(false)
{
    
}

MapUIUnitNode::~MapUIUnitNode()
{
    removeAllChildren();
}

bool MapUIUnitNode::init(const Camp* camp, ssize_t idx)
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
        const string& unitName = camp ? camp->getUnitSetting().getUnitTypeName() : WOLF_SOLDIER;
        const string& iconFile = getIconFile(unitName);
        _iconButton = Button::create(iconFile, iconFile);
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
        
        if (camp) {
            const std::map<std::string, int>* costs = camp->getCosts();
            if (costs) {
                if (costs->find(RES_NAME_GOLD) != costs->end()) {
                    _resourceButton = ResourceButton::create(false, kResourceType_Gold, costs->at(RES_NAME_GOLD), nullptr);
                    addChild(_resourceButton);
                }
            }
        } else {
            _resourceButton = ResourceButton::create(false, kResourceType_Gold, 100, nullptr);
            addChild(_resourceButton);
        }
        
        _countLabel = CocosUtils::createLabel("0", DEFAULT_FONT_SIZE);
        addChild(_countLabel);
        
        const float iconHeight = _iconButton->getContentSize().height;
        const float resourceHeight = _resourceButton->getContentSize().height;
        const float countHeight = _countLabel->getContentSize().height;
        static const float offsetY(2.0f);
#if true
        const Size size(_iconButton->getContentSize().width, iconHeight);
        setContentSize(size);
        
        const float x = size.width / 2;
        const float y = size.height / 2;
        _iconButton->setPosition(Point(x, y));
        _resourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _resourceButton->setPosition(Point(x, resourceHeight / 2 + offsetY));
        _countLabel->setPosition(Point(x, size.height - countHeight / 2 - offsetY));
#else
        const Size size(_iconButton->getContentSize().width, iconHeight + resourceHeight + countHeight + offsetY * 4);
        setContentSize(size);
        
        const float x = size.width / 2;
        const float y = size.height / 2;
        _iconButton->setPosition(Point(x, y));
        _resourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _resourceButton->setPosition(Point(x, y - (iconHeight + resourceHeight) / 2 - offsetY));
        _countLabel->setPosition(Point(x, y + (iconHeight + countHeight) / 2 + offsetY));
#endif
        
        if (camp) {
            reuse(camp, idx);
        }
#endif
        
        return true;
    }
    
    return false;
}

void MapUIUnitNode::registerObserver(MapUIUnitNodeObserver *observer)
{
    _observer = observer;
}

void MapUIUnitNode::reuse(const Camp* camp, ssize_t idx)
{
    _camp = camp;
    _idx = idx;
    
    const string& unitName = camp ? camp->getUnitSetting().getUnitTypeName() : "";
    const string& iconFile = getIconFile(unitName);
    if (iconFile.length() > 0) {
        _iconButton->loadTextures(iconFile, iconFile);
    }
    
    // update mutable data
    update(true);
}

void MapUIUnitNode::update(bool reuse)
{
    if (_camp) {
        _countLabel->setString(StringUtils::format("%d/%d", _camp->getProduction(), _camp->getMaxProduction()));
        
        const std::map<std::string, int>* costs = _camp->getCosts();
        if (costs) {
            if (costs->find(RES_NAME_GOLD) != costs->end()) {
                _resourceButton->setCount(costs->at(RES_NAME_GOLD));
            }
        }
    }
    
    if (!reuse) {
        if (_observer) {
            _observer->onMapUIUnitNodeUpdated(this);
        }
    }
}

void MapUIUnitNode::setSelected(bool selected)
{
    if (_iconButton) {
        Node *parent = _iconButton->getParent();
        if (parent) {
            static const int selectedTag(100);
            Node *selectedSprite = parent->getChildByTag(selectedTag);
            if (selected) {
                if (!selectedSprite) {
                    Sprite *s = Sprite::create("GameImages/test/ui_xuanzhong.png");
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

const Camp* MapUIUnitNode::getCamp() const
{
    return _camp;
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
,_world(nullptr)
,_paused(false)
,_cellsCount(3)
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
,_waveTime(11)  // TODO: reset wave time
,_remainingTime(battleTotalTime)
{
    static const Size& unitNodeSize = MapUIUnitNode::create(nullptr, 0)->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY * 2;
    _cellSize.width = unitNodeSize.width + unitNodeOffsetX;
    
    _tableViewMaxSize.width = _cellSize.width * visibleCellsCount + unitNodeOffsetX;
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
        _world = game->getWorld();
        if (_world) {
            const int cnt = _world->getFactionCount();
            for (int i = 0; i < cnt; ++i) {
                const Faction* faction = _world->getFaction(i);
                _cores.insert(make_pair(faction->getFactionIndex(), faction->findFirstUnitByClass(kUnitClass_Core)));
            }
            
            updateResources();
            
            // reload table view
            reloadTableView(_world->getCampCount(_world->getThisFactionIndex()));
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

#pragma mark - TableViewDataSource
Size MapUILayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return _cellSize;
}

TableViewCell* MapUILayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    MapUIUnitCell *cell = static_cast<MapUIUnitCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = MapUIUnitCell::create();
    }
    
    if (_world) {
        const int factionIndex = _world->getThisFactionIndex();
        const Camp* camp = _world->getCamp(factionIndex, (int)idx);
        if (camp) {
            MapUIUnitNode* unitNode = cell->getUnitNode();
            if (unitNode) {
                unitNode->reuse(camp, idx);
                unitNode->setSelected(idx == _selectedUnitIdx);
            } else {
                unitNode = MapUIUnitNode::create(camp, idx);
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
    return _cellsCount;
}

#pragma mark - MapUIUnitNodeObserver
void MapUILayer::onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node)
{
    onUnitTouched(node);
}

void MapUILayer::onMapUIUnitNodeUpdated(MapUIUnitNode* node)
{
    
}

#pragma mark -
bool MapUILayer::init(const string& myAccount, const string& opponentsAccount)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
#if false
        static const string file("GameImages/test/test_ui_bg.png");
        Sprite *bg = Sprite::create(file);
        addChild(bg, 1);
        
        const Size& size = bg->getContentSize();
        bg->setPosition(Point(size.width / 2, size.height / 2));
        bg->setScale(size.width / winSize.width, size.height / winSize.height);
        
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
        
#elif false
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
        static const float leftOffset(20.0f);
        static const float ceilOffset(10.0f);
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
            
            _populationLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
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
            const Point& pos = Point(winSize.width / 2, ceilOffset);
#if false
            Sprite* sprite = CocosUtils::createPureColorSprite(_tableViewMaxSize, Color4B::BLACK);
            sprite->setPosition(pos + Point(0, _tableViewMaxSize.height / 2));
            root->addChild(sprite);
#endif
            
            _tableView = TableView::create(this, _tableViewMaxSize);
            _tableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
            _tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
            _tableView->setPosition(pos - Point(_tableViewMaxSize.width / 2, unitNodeOffsetY));
            _tableView->setBounceable(false);
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
        
        updateWaveTime(_waveTime);
        updateRemainingTime(_remainingTime);
        updateMyHpProgress(60);
        updateOpponentsHpProgress(20);
        updateResources();
#endif
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [=](Touch *touch, Event *unused_event) {
            const Point& p = touch->getLocation();
            if (_tableView && _tableView->getBoundingBox().containsPoint(p)) {
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
    if (_nextWaveTimeLabel) {
        _scheduler->schedule(schedule_selector(MapUILayer::fakeTick), this, 1.0f, false);
    }
}

void MapUILayer::onExit()
{
    if (_nextWaveTimeLabel) {
        _scheduler->unschedule(schedule_selector(MapUILayer::fakeTick), this);
    }
    LayerColor::onExit();
}

#pragma mark private
void MapUILayer::onUnitTouched(MapUIUnitNode* node)
{
    const ssize_t oldIdx = _selectedUnitIdx;
    const ssize_t newIdx = node->getIdx();
    if (newIdx != oldIdx) {
        _selectedUnitIdx = newIdx;
        
        if (oldIdx != CC_INVALID_INDEX) {
            _tableView->updateCellAtIndex(oldIdx);
        }
        
        _tableView->updateCellAtIndex(newIdx);
    }
    
    if (_observer) {
        _observer->onMapUILayerUnitSelected(node);
    }
}

void MapUILayer::reloadTableView(ssize_t cellsCount)
{
    _cellsCount = cellsCount;
    if (_tableView) {
        // if setTouchEnabled to false, tableCellTouched() will never be called
        _tableView->setTouchEnabled(_cellsCount > visibleCellsCount);
        _tableView->reloadData();
        
        // fit
        if (false == _tableView->isBounceable() && extension::ScrollView::Direction::HORIZONTAL == _tableView->getDirection()) {
            _tableView->getContainer()->setPosition(Point::ZERO);
            const int contentWidth = _cellSize.width * _cellsCount + unitNodeOffsetX;
            if (contentWidth >= _tableViewMaxSize.width) {
                _tableView->setViewSize(_tableViewMaxSize);
            } else {
                _tableView->setViewSize(Size(contentWidth, _tableViewMaxSize.height));
            }
        }
    }
}

void MapUILayer::updateResources()
{
    if (_world) {
        const TechTree* techTree = _world->getTechTree();
        const Faction* faction = _world->getThisFaction();
        int count = techTree->getResourceTypeCount();
        for (int i = 0; i < count; ++i) {
            const UnderWorld::Core::ResourceType* resourceType = techTree->getResourceTypeByIndex(i);
            const Resource* resource = faction->getResource(resourceType);
            if (kResourceClass_holdable == resourceType->_class) {
                _populationLabel->setString(StringUtils::format("%d/%d", resource->getOccpied(), resource->getBalance()));
            } else {
                _energyResourceButton->setCount(resource->getBalance());
            }
        }
    }
    
}

void MapUILayer::fakeTick(float dt)
{
    -- _waveTime;
    -- _remainingTime;
    
    if (_waveTime <= 0) {
        _waveTime = waveTime;
    }
    
    if (_remainingTime <= 0) {
        _remainingTime = battleTotalTime;
    }
    
    updateWaveTime(_waveTime);
    updateRemainingTime(_remainingTime);
    updateResources();    
    
    if (_world) {
        for (map<int, Unit*>::iterator iter = _cores.begin(); iter != _cores.end(); ++iter) {
            const Unit* core(iter->second);
            if (core) {
                const int maxHp = core->getUnitType()->getMaxHp();
                const int hp = core->getHp();
                const float percentage = 100 * (float)hp / (float)maxHp;
                if (iter->first == _world->getThisFactionIndex()) {
                    updateMyHpProgress(percentage);
                } else {
                    updateOpponentsHpProgress(percentage);
                }
            }
        }
    }
}