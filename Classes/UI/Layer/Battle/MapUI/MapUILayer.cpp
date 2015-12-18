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
#include "MapUIUnitCell.h"
#include "ResourceButton.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const float unitNodeOffsetX(17.0f);
static const float unitNodeOffsetY(17.0f);

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
,_pauseMenuItem(nullptr)
{
    static const Size unitNodeSize = MapUIUnitNode::create(nullptr, 0)->getContentSize();
    _cellSize.height = unitNodeSize.height + unitNodeOffsetY * 2;
    _cellSize.width = unitNodeSize.width + unitNodeOffsetX;
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
    if (_tableView) {
        if (_observer) {
            _cellsCount = _observer->onMapUILayerCampsCount();
        }
        
        // if setTouchEnabled to false, tableCellTouched() will never be called
        const int visibleCellsCount = (_tableViewMaxSize.width - unitNodeOffsetX) / _cellSize.width;
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

void MapUILayer::updatePopulation(int count, int maxCount)
{
    _populationLabel->setString(StringUtils::format("%d/%d", count, maxCount));
}

void MapUILayer::updateGold(int count)
{
    _energyResourceButton->setCount(count);
}

void MapUILayer::pauseGame()
{
    
}

void MapUILayer::resumeGame()
{
    
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
    
    if (_observer) {
        const Camp* camp = _observer->onMapUILayerCampAtIndex(idx);
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
            
            // units table
            const Point& pos = sprite->getPosition() + Point(sprite->getContentSize().width + leftOffset, - unitNodeOffsetY);
            
            _tableViewMaxSize.width = winSize.width - (pos.x + leftOffset);
            _tableViewMaxSize.height = _cellSize.height;
            
            _tableView = TableView::create(this, _tableViewMaxSize);
            _tableView->setDirection(extension::ScrollView::Direction::HORIZONTAL);
            _tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
            _tableView->setPosition(pos);
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
                if (_observer) {
                    _observer->onMapUILayerClickedPauseButton();
                }
            });
            _pauseMenuItem->setAnchorPoint(Point(1.0f, 1.0f));
            _pauseMenuItem->setPosition(Point(winSize.width - 5.0f, winSize.height - 5.0f));
            Menu *menu = Menu::create(_pauseMenuItem, nullptr);
            menu->setPosition(Point::ZERO);
            root->addChild(menu);
        }
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
    
    SoundManager::getInstance()->playButtonSelectUnitSound();
    
    if (_observer) {
        _observer->onMapUILayerUnitSelected(node);
    }
}