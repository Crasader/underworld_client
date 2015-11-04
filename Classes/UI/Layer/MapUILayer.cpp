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

using namespace std;
using namespace cocostudio;

#pragma mark =====================================================
#pragma mark Unit Node
#pragma mark =====================================================

MapUIUnitNode* MapUIUnitNode::create()
{
    MapUIUnitNode *ret = new (nothrow) MapUIUnitNode();
    if (ret && ret->init())
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
{
    
}

MapUIUnitNode::~MapUIUnitNode()
{
    removeAllChildren();
}

bool MapUIUnitNode::init()
{
    if (Node::init())
    {
        const Size& winSize = Director::getInstance()->getWinSize();
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
        
        return true;
    }
    
    return false;
}

void MapUIUnitNode::registerObserver(MapUIUnitNodeObserver *observer)
{
    _observer = observer;
}

#pragma mark =====================================================
#pragma mark MapUILayer
#pragma mark =====================================================

static Sprite* createDemoBg(uint width, uint height, const Color4B& color)
{
    // 1. create texture
    static const uint a = 4;
    char data[width][height][a];
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            data[i][j][0] = color.r;
            data[i][j][0] = color.g;
            data[i][j][0] = color.b;
            data[i][j][0] = color.a;
        }
    }
    
    Texture2D* texture = new (nothrow) Texture2D();
    if (texture) {
        bool ret = texture->initWithData(data, width * height * a, Texture2D::PixelFormat::RGBA4444, width, height, Size(width, height));
        if (ret) {
            texture->autorelease();
        } else {
            CC_SAFE_DELETE(texture);
        }
    }
    
    // 2. create sprite
    Sprite* sprite = Sprite::createWithTexture(texture);
    return sprite;
}

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
    
}

MapUILayer::~MapUILayer()
{
    removeAllChildren();
}

void MapUILayer::registerObserver(MapUILayerObserver *observer)
{
    _observer = observer;
}

#pragma mark - TableViewDataSource
void MapUILayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    
}

#pragma mark - TableViewDelegate
Size MapUILayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return Size(618, 60);
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
    
    Node* node = nullptr;
    
    const Size cellSize(tableCellSizeForIndex(_tableView, 0));
    node->setPosition(Point(_tableView->getContentSize().width / 2, cellSize.height / 2));
    node->setTag(NodeTag);
    cell->addChild(node);
    
    return cell;
}

ssize_t MapUILayer::numberOfCellsInTableView(TableView *table)
{
    return 10;
}

#pragma mark -
bool MapUILayer::init(const std::string& myAccount, const std::string& opponentsAccount)
{
    if (LayerColor::initWithColor(Color4B(0, 0, 0, 128)))
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
        //
        {
            Sprite* sprite = createDemoBg(200, 100, Color4B::BLACK);
            sprite->setPosition(Point(100, 600));
            root->addChild(sprite);
            const Size& size = sprite->getContentSize();
            Label* label = CocosUtils::createLabel("下一波进攻时间", BIG_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            _nextWaveTimeLabel = CocosUtils::createLabel(StringUtils::format("%d", 20), DEFAULT_FONT_SIZE);
            _nextWaveTimeLabel->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_nextWaveTimeLabel);
        }
        //
        {
            Sprite* sprite = createDemoBg(200, 100, Color4B::BLACK);
            sprite->setPosition(Point(100, 600));
            root->addChild(sprite);
            const Size& size = sprite->getContentSize();
            Label* label = CocosUtils::createLabel("我的名字", BIG_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            Sprite* s = createDemoBg(200, 40, Color4B::GREEN);
            _myHpProgress = ProgressTimer::create(s);
            _myHpProgress->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_myHpProgress);
            
            _myHpPercentageLabel = CocosUtils::createLabel(StringUtils::format("%d%%", 20), DEFAULT_FONT_SIZE);
            _myHpPercentageLabel->setPosition(_myHpProgress->getPosition());
            sprite->addChild(_myHpPercentageLabel);
        }
        //
        {
            Sprite* sprite = createDemoBg(200, 100, Color4B::BLACK);
            sprite->setPosition(Point(100, 600));
            root->addChild(sprite);
            const Size& size = sprite->getContentSize();
            Label* label = CocosUtils::createLabel("战斗时间", BIG_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            _timeLabel = CocosUtils::createLabel(StringUtils::format("%d", 20), DEFAULT_FONT_SIZE);
            _timeLabel->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_timeLabel);
        }
        //
        {
            Sprite* sprite = createDemoBg(200, 100, Color4B::BLACK);
            sprite->setPosition(Point(100, 600));
            root->addChild(sprite);
            const Size& size = sprite->getContentSize();
            Label* label = CocosUtils::createLabel("对手的名字", BIG_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            Sprite* s = createDemoBg(200, 40, Color4B::GREEN);
            _opponentsHpProgress = ProgressTimer::create(s);
            _opponentsHpProgress->setPosition(Point(size.width / 2, size.height * 0.25f));
            sprite->addChild(_opponentsHpProgress);
            
            _opponentsHpPercentageLabel = CocosUtils::createLabel(StringUtils::format("%d%%", 20), DEFAULT_FONT_SIZE);
            _opponentsHpPercentageLabel->setPosition(_opponentsHpProgress->getPosition());
            sprite->addChild(_opponentsHpPercentageLabel);
        }
        // units table
        {
            Sprite* sprite = createDemoBg(200, 100, Color4B::BLACK);
            sprite->setPosition(Point(100, 600));
            root->addChild(sprite);
            const Size& size = sprite->getContentSize();
            Label* label = CocosUtils::createLabel("能量", BIG_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.95));
            sprite->addChild(label);
            _energyLabel = CocosUtils::createLabel(StringUtils::format("%d", 2000), BIG_FONT_SIZE);
            _energyLabel->setPosition(Point(size.width / 2, size.height * 0.85));
            sprite->addChild(_energyLabel);
            label = CocosUtils::createLabel("出战单位", BIG_FONT_SIZE);
            label->setPosition(Point(size.width / 2, size.height * 0.75));
            sprite->addChild(label);
            
            _tableView = TableView::create(this, Size(200, 388));
            _tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
            _tableView->setPosition(Point(0, 95));
            _tableView->setBounceable(false);
            _tableView->setDelegate(this);
            root->addChild(_tableView);
        }
        // buttons
        {
            Sprite* sprite = createDemoBg(200, 100, Color4B::ORANGE);
            sprite->setPosition(Point(100, 600));
            const Size& size = sprite->getContentSize();
            _unitCostLabel = CocosUtils::createLabel(StringUtils::format("%d", 100), DEFAULT_FONT_SIZE);
            _unitCostLabel->setPosition(Point(size.width / 2, size.height / 2));
            sprite->addChild(_unitCostLabel);
            _sendTroopMenuItem = MenuItemSprite::create(sprite, sprite);
            _sendTroopMenuItem->setPosition(Point(200, 100));
            _pauseMenuItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png");
            _pauseMenuItem->setPosition(Point(1000, 600));
            Menu *menu = Menu::create(_sendTroopMenuItem, _pauseMenuItem, nullptr);
            root->addChild(menu);
        }
#endif
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [](Touch *touch, Event *unused_event) {
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