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

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const Size unitIconSize(50, 50);

static Sprite* createPureColorSprite(const Size& size, const Color4B& color)
{
    // 1. create texture
    static const uint len = 4;
    GLubyte data[len] = {color.r, color.g, color.b, 180};
    
    Texture2D* texture = new (nothrow) Texture2D();
    if (texture) {
        bool ret = texture->initWithData(data, len, Texture2D::PixelFormat::RGBA8888, 1, 1, Size(1, 1));
        if (ret) {
            texture->autorelease();
        } else {
            CC_SAFE_DELETE(texture);
        }
    }
    
    // 2. create sprite
    Sprite* sprite = Sprite::createWithTexture(texture);
    sprite->setScale(size.width, size.height);
    
    // 3.you need to create a new sprite, you cannot change the contentSize of the old sprite directly
    Sprite* newSprite = Sprite::create();
    newSprite->setContentSize(size);
    sprite->setPosition(Point(size.width / 2, size.height / 2));
    newSprite->addChild(sprite);
    
    return newSprite;
}

#pragma mark =====================================================
#pragma mark Unit Node
#pragma mark =====================================================

MapUIUnitNode* MapUIUnitNode::create(const UnitType* type)
{
    MapUIUnitNode *ret = new (nothrow) MapUIUnitNode();
    if (ret && ret->init(type))
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
,_unitType(nullptr)
{
    
}

MapUIUnitNode::~MapUIUnitNode()
{
    removeAllChildren();
}

bool MapUIUnitNode::init(const UnitType* type)
{
    if (Node::init())
    {
        _unitType = type;
        
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
        Button* icon = Button::create("GameImages/touxiang.png", "GameImages/touxiang.png");
        icon->setPressedActionEnabled(false);
        icon->setSwallowTouches(false);
        addChild(icon);
        
        ResourceButton *button = ResourceButton::create(false, kResourceType_Gold, 100, nullptr);
        addChild(button);
        
        const float rootHeight = icon->getContentSize().height;
        const float buttonHeight = button->getContentSize().height;
        const Size size(icon->getContentSize().width + 5.0f, rootHeight + buttonHeight + 5.0f);
        setContentSize(size);
        icon->setPosition(Point(size.width / 2, size.height / 2 + (rootHeight + buttonHeight) / 4));
        button->setPosition(Point(size.width / 2, size.height / 2 - (rootHeight + buttonHeight) / 4));
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

#pragma mark =====================================================
#pragma mark MapUILayer
#pragma mark =====================================================

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
,_energyLabel(nullptr)
,_unitCostLabel(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_sendTroopMenuItem(nullptr)
,_pauseMenuItem(nullptr)
{
    _unitNodeSize = MapUIUnitNode::create(nullptr)->getContentSize();
    _cellSize.width = (_unitNodeSize.width + 10.0f) * columnCount;
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
        ssize_t index = idx * columnCount + i;
        MapUIUnitNode* unitNode = MapUIUnitNode::create(nullptr);
        unitNode->setPosition(Point(_unitNodeSize.width * i, offsetY));
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

#pragma mark MapUIUnitNodeObserver
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
            static const Size size(180, 60);
            Sprite* sprite = createPureColorSprite(size, Color4B::BLACK);
            sprite->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            sprite->setPosition(Point(leftOffset, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            Label* label = CocosUtils::createLabel("下一波进攻时间", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _nextWaveTimeLabel = CocosUtils::createLabel(StringUtils::format("%ds", 20), DEFAULT_FONT_SIZE);
            _nextWaveTimeLabel->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_nextWaveTimeLabel);
        }
        //
        {
            static const Size size(180, 60);
            Sprite* sprite = createPureColorSprite(size, Color4B::BLACK);
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2 - 150.0f, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            Label* label = CocosUtils::createLabel(myAccount, DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            Sprite* s = createPureColorSprite(Size(200, 40), Color4B::GREEN);
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
            static const Size size(100, 60);
            Sprite* sprite = createPureColorSprite(size, Color4B::BLACK);
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            Label* label = CocosUtils::createLabel("战斗时间", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _timeLabel = CocosUtils::createLabel(CocosUtils::getFormattedTime(battleTotalTime), DEFAULT_FONT_SIZE);
            _timeLabel->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_timeLabel);
        }
        //
        {
            static const Size size(180, 60);
            Sprite* sprite = createPureColorSprite(size, Color4B::BLACK);
            sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
            sprite->setPosition(Point(winSize.width / 2 + 150.0f, winSize.height - ceilOffset));
            root->addChild(sprite);
            
            Label* label = CocosUtils::createLabel(opponentsAccount, DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            Sprite* s = createPureColorSprite(progressSize, Color4B::GREEN);
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
            static const Size size(180, winSize.height - 180);
            Sprite* sprite = createPureColorSprite(size, Color4B::BLACK);
            sprite->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
            sprite->setPosition(Point(leftOffset, winSize.height - (90.0f + ceilOffset)));
            root->addChild(sprite);
            
            Label* label = CocosUtils::createLabel("能量", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height - 12));
            sprite->addChild(label);
            
            _energyLabel = CocosUtils::createLabel(StringUtils::format("%d", 2000), DEFAULT_FONT_SIZE);
            _energyLabel->setPosition(Point(size.width / 2, size.height - 36));
            sprite->addChild(_energyLabel);
            
            label = CocosUtils::createLabel("出战单位", DEFAULT_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height - 60));
            sprite->addChild(label);
            
            _tableView = TableView::create(this, Size(_cellSize.width, size.height - 80));
            _tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
            _tableView->setPosition(Point(leftOffset + (size.width - _cellSize.width) / 2, 85));
            _tableView->setBounceable(false);
            _tableView->setDelegate(this);
            root->addChild(_tableView);
        }
        // buttons
        {
            static const Size size(180, 60);
            Sprite* sprite = createPureColorSprite(size, Color4B::ORANGE);
            _unitCostLabel = CocosUtils::createLabel(StringUtils::format("%d", 100), DEFAULT_FONT_SIZE);
            _unitCostLabel->setPosition(Point(size.width / 2, size.height / 2));
            sprite->addChild(_unitCostLabel);
            _sendTroopMenuItem = MenuItemSprite::create(sprite, sprite, [](Ref*) {
                CCLOG("Sending troop...");
            });
            _sendTroopMenuItem->setAnchorPoint(Point::ZERO);
            _sendTroopMenuItem->setPosition(Point(leftOffset, 5.0f));
            _pauseMenuItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", [](Ref*) {
                
            });
            _pauseMenuItem->setAnchorPoint(Point(1.0f, 1.0f));
            _pauseMenuItem->setPosition(Point(winSize.width - 5.0f, winSize.height - 5.0f));
            Menu *menu = Menu::create(_sendTroopMenuItem, _pauseMenuItem, nullptr);
            menu->setPosition(Point::ZERO);
            root->addChild(menu);
        }
#endif
        
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
}