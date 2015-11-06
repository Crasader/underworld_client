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
#include "UnitType.h"
#include "ResourceButton.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const Size unitIconSize(50, 50);
static const int selectedTag = 100;

#pragma mark =====================================================
#pragma mark Unit Node
#pragma mark =====================================================

MapUIUnitNode* MapUIUnitNode::create(const UnderWorld::Core::UnitType* type, ssize_t idx)
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
,_unitType(nullptr)
,_touchInvalid(false)
{
    
}

MapUIUnitNode::~MapUIUnitNode()
{
    removeAllChildren();
}

bool MapUIUnitNode::init(const UnderWorld::Core::UnitType* type, ssize_t idx)
{
    if (Node::init())
    {
        _unitType = type;
        _idx = idx;
        
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
        const string iconFile = (type == nullptr) ? "GameImages/test/icon_jz.png" : "GameImages/test/icon_gjs.png";
        _iconButton = Button::create(iconFile, iconFile);
        _iconButton->setPressedActionEnabled(false);
        _iconButton->setSwallowTouches(false);
        addChild(_iconButton);
        _iconButton->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            Widget* button = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                _touchInvalid = false;
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedBegan(this);
                }
            } else if (type == Widget::TouchEventType::MOVED) {
                if (_touchInvalid) return;
                if (button->getTouchMovePosition().distance(button->getTouchBeganPosition()) > TOUCH_CANCEL_BY_MOVING_DISTANCE) {
                    _touchInvalid = true;
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                if (_touchInvalid) return;
                SoundManager::getInstance()->playButtonSound();
                if(_observer) {
                    _observer->onMapUIUnitNodeTouchedEnded(this);
                }
            }
        });
        
        ResourceButton *button = ResourceButton::create(false, kResourceType_Gold, 100, nullptr);
        addChild(button);
        
        const float rootHeight = _iconButton->getContentSize().height;
        const float buttonHeight = button->getContentSize().height;
        const Size size(_iconButton->getContentSize().width + 2.0f, rootHeight + buttonHeight + 5.0f);
        setContentSize(size);
        
        _iconButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _iconButton->setPosition(Point(size.width / 2, size.height - rootHeight / 2));
        button->setAnchorPoint(Point::ANCHOR_MIDDLE);
        button->setPosition(Point(size.width / 2, buttonHeight / 2));
#endif
        
        return true;
    }
    
    return false;
}

void MapUIUnitNode::registerObserver(MapUIUnitNodeObserver *observer)
{
    _observer = observer;
}

const UnderWorld::Core::UnitType* MapUIUnitNode::getUnitType() const
{
    return _unitType;
}

ssize_t MapUIUnitNode::getIdx() const
{
    return _idx;
}

void MapUIUnitNode::addSelectedFlag()
{
    if (_iconButton) {
        Node *parent = _iconButton->getParent();
        Node *selectedSprite = parent->getChildByTag(selectedTag);
        if (!selectedSprite) {
            Sprite *s = Sprite::create("GameImages/formation/Selected.png");
            s->setTag(selectedTag);
            s->setPosition(_iconButton->getPosition());
            parent->addChild(s);
        }
    }
}

void MapUIUnitNode::removeSelectedFlag()
{
    if (_iconButton) {
        Node *parent = _iconButton->getParent();
        if (parent) {
            parent->removeChildByTag(selectedTag);
        }
    }
}

#pragma mark =====================================================
#pragma mark MapUILayer
#pragma mark =====================================================

static const int waveTime = 20;
static const int battleTotalTime = 180;
static const uint columnCount = 2;
static const float offsetY = 12;

MapUILayer* MapUILayer::create(const std::string& myAccount, const std::string& opponentsAccount)
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
,_tableView(nullptr)
,_timeLabel(nullptr)
,_nextWaveTimeLabel(nullptr)
,_energyResourceButton(nullptr)
,_unitCostLabel(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_sendTroopMenuItem(nullptr)
,_pauseMenuItem(nullptr)
,_waveTime(waveTime)
,_remainingTime(battleTotalTime)
,_selectedUnitIdx(-1)
{
    _unitNodeSize = MapUIUnitNode::create(nullptr, 0)->getContentSize();
    _cellSize.width = _unitNodeSize.width * columnCount;
    _cellSize.height = _unitNodeSize.height + offsetY * 2;
}

MapUILayer::~MapUILayer()
{
    removeAllChildren();
}

void MapUILayer::registerObserver(MapUILayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - TableViewDelegate
void MapUILayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    
}

#pragma mark - TableViewDataSource
Size MapUILayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return _cellSize;
}

TableViewCell* MapUILayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    static const int NodeTag = 100;
    TableViewCell *cell = table->dequeueCell();
    if (!cell)
    {
        cell = new (nothrow) TableViewCell();
        cell->autorelease();
    }
    else
    {
        cell->removeChildByTag(NodeTag);
    }
    
    Node* node = Node::create();
    
    for (int i = 0; i < columnCount; ++i)
    {
//        ssize_t index = idx * columnCount + i;
        UnitType* type = (i == 0) ? nullptr : reinterpret_cast<UnitType*>(1);
        MapUIUnitNode* unitNode = MapUIUnitNode::create(type, idx);
        unitNode->setPosition(Point(_unitNodeSize.width * i, offsetY ));
        unitNode->registerObserver(this);
        node->addChild(unitNode);
    }

    node->setPosition(Point::ZERO);
    node->setTag(NodeTag);
    cell->addChild(node);
    
    return cell;
}

ssize_t MapUILayer::numberOfCellsInTableView(TableView *table)
{
    return 10;
}

#pragma mark - MapUIUnitNodeObserver
void MapUILayer::onMapUIUnitNodeTouchedBegan(MapUIUnitNode* node)
{
    
}

void MapUILayer::onMapUIUnitNodeTouchedEnded(MapUIUnitNode* node)
{
    
}

#pragma mark -
bool MapUILayer::init(const std::string& myAccount, const std::string& opponentsAccount)
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
            
            Sprite* s = Sprite::create("GameImages/test/ui_xt_1.png");
            _myHpProgress = ProgressTimer::create(s);
            _myHpProgress->setPercentage(100);
            _myHpProgress->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_myHpProgress);
            
            _myHpPercentageLabel = CocosUtils::createLabel(StringUtils::format("%d%%", 20), DEFAULT_FONT_SIZE);
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
            
            Sprite* s = Sprite::create("GameImages/test/ui_xt_1.png");
            _opponentsHpProgress = ProgressTimer::create(s);
            _opponentsHpProgress->setPercentage(100);
            _opponentsHpProgress->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_opponentsHpProgress);
            
            _opponentsHpPercentageLabel = CocosUtils::createLabel(StringUtils::format("%d%%", 20), DEFAULT_FONT_SIZE);
            _opponentsHpPercentageLabel->setPosition(_opponentsHpProgress->getPosition());
            sprite->addChild(_opponentsHpPercentageLabel);
        }
        // units table
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_cd.png");
            sprite->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            sprite->setPosition(Point(leftOffset, winSize.height - (90.0f + ceilOffset)));
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            
            Label* label = CocosUtils::createLabel("能量", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height - 12));
            sprite->addChild(label);
            
            _energyResourceButton = ResourceButton::create(false, kResourceType_Gold, 2000, nullptr);
            _energyResourceButton->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _energyResourceButton->setPosition(Point(size.width / 2, size.height - 36));
            sprite->addChild(_energyResourceButton);
            
            label = CocosUtils::createLabel("出战单位", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height - 60));
            sprite->addChild(label);
            
            _tableView = TableView::create(this, Size(_cellSize.width, size.height - 80));
            _tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
            _tableView->setPosition(Point(leftOffset + (size.width - _cellSize.width) / 2, 120));
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
//            _sendTroopMenuItem = MenuItemSprite::create(sprite, sprite, [](Ref*) {
//                CCLOG("Sending troop...");
//            });
//            _sendTroopMenuItem->setAnchorPoint(Point::ZERO);
//            _sendTroopMenuItem->setPosition(Point(leftOffset, 5.0f));
            _pauseMenuItem = MenuItemImage::create("GameImages/test/ui_zt.png", "GameImages/test/ui_zt.png", [](Ref*) {
                
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

void MapUILayer::updateWaveTime(int time)
{
    _nextWaveTimeLabel->setString(StringUtils::format("%ds", time));
}

void MapUILayer::updateRemainingTime(int time)
{
    _timeLabel->setString(CocosUtils::getFormattedTime(time));
}