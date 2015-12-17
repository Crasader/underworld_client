//
//  BagLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BagLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "BagCell.h"
#include "TabButton.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

static const float nodeOffsetX(17.0f);
static const float nodeOffsetY(17.0f);
static const int visibleCellsCount(6);

BagLayer* BagLayer::create(int tabIndex)
{
    BagLayer *ret = new (nothrow) BagLayer();
    if (ret && ret->init(tabIndex))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BagLayer::BagLayer()
:_observer(nullptr)
,_scrollBar(nullptr)
,_tabIndex(-1)
{
    static const Size unitNodeSize = Size::ZERO;
    _cellSize.height = unitNodeSize.height + nodeOffsetY * 2;
    _cellSize.width = unitNodeSize.width + nodeOffsetX;
    
    _tableViewMaxSize.width = _cellSize.width * visibleCellsCount + nodeOffsetX;
    _tableViewMaxSize.height = _cellSize.height;
}

BagLayer::~BagLayer()
{
    removeAllChildren();
}

void BagLayer::registerObserver(BagLayerObserver *observer)
{
    _observer = observer;
}

bool BagLayer::init(int tabIndex)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        
        static const string CsbFile("zhuchangjing.csb");
        Node* mainNode = CSLoader::createNode(CsbFile);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        
        Node* root = mainNode->getChildByTag(7);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                        case 100:
                        {
                            Node* node = child->getChildByTag(8);
                            if (node) {
                                Label* label = CocosUtils::createLabel(LocalHelper::getString("ui_Bag_title"), DEFAULT_FONT_SIZE);
                                node->addChild(label);
                            }
                        }
                            break;
                        case 101:
                        {
                            addTabButton(child, LocalHelper::getString(""), 0, [](Ref*) {
                                
                            });
                        }
                            break;
                        case 102:
                        {
                            addTabButton(child, LocalHelper::getString(""), 1, [](Ref*) {
                                
                            });
                        }
                            break;
                        case 103:
                        {
                            addTabButton(child, LocalHelper::getString(""), 2, [](Ref*) {
                                
                            });
                        }
                            break;
                        case 104:
                        {
                            Button* button = dynamic_cast<Button*>(child);
                            if (button) {
                                button->setPressedActionEnabled(true);
                                button->addClickEventListener([this](Ref *pSender){
                                    SoundManager::getInstance()->playButtonSound();
                                    if (_observer) {
                                        _observer->onBagLayerClosed();
                                    }
                                    removeFromParent();
                                });
                            }
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        switchTable(tabIndex);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(BagLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(BagLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool BagLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void BagLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDelegate
Size BagLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return _cellSize;
}

TableViewCell* BagLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    BagCell *cell = static_cast<BagCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = BagCell::create();
    }
    
    BagNode* node = cell->getNode();
    if (node) {
        
    } else {
        
    }
    
    return cell;
}

ssize_t BagLayer::numberOfCellsInTableView(TableView *table)
{
    return 1;
}

void BagLayer::addTabButton(Node* parent, const string& title, int tabIndex, const Button::ccWidgetClickCallback& callback)
{
    TabButton* button = TabButton::create(title, callback);
    parent->addChild(button);
    if (_tabInfos.find(tabIndex) != _tabInfos.end()) {
        assert(false);
    } else {
        _tabInfos.insert(make_pair(tabIndex, TabInfo{nullptr, button}));
    }
}

void BagLayer::addTableView(int index)
{
    if (_tabInfos.find(index) != _tabInfos.end()) {
        TabInfo& info = _tabInfos.at(index);
        if (nullptr == info.tableView) {
            TableView* tableView = TableView::create(this, _tableViewMaxSize);
            tableView->setDirection(extension::ScrollView::Direction::VERTICAL);
            tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
            tableView->setPosition(Point::ZERO);
            tableView->setBounceable(false);
            info.tableView = tableView;
        }
    }
}

void BagLayer::switchTable(int index)
{
    if (_tabIndex != index) {
        _tabIndex = index;
        for (map<int, TabInfo>::iterator iter = _tabInfos.begin(); iter != _tabInfos.end(); ++iter) {
            const TabInfo& info = iter->second;
            bool isThisIndex = (index == iter->first) ? true : false;
            if (info.tableView) {
                info.tableView->setVisible(isThisIndex);
            } else if (isThisIndex) {
                addTableView(index);
            }
            info.tabButton->setEnabled(!isThisIndex);
        }
    }
}
