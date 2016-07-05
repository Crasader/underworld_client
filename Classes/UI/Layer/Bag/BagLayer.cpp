//
//  BagLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BagLayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "XTableViewCell.h"
#include "TabButton.h"

using namespace std;
using namespace ui;

static const float nodeOffsetX(17.0f);
static const float nodeOffsetY(17.0f);
static const int visibleCellsCount(6);
static const unsigned int columnCount(4);

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
    static const Size unitNodeSize = BagNode::create(nullptr, 0, 0)->getContentSize();
    _cellSize.height = unitNodeSize.height + nodeOffsetY * 2;
    _cellSize.width = (unitNodeSize.width + nodeOffsetX) * columnCount + nodeOffsetX;
    
    _tableViewMaxSize.width = _cellSize.width;
    _tableViewMaxSize.height = _cellSize.height * visibleCellsCount + nodeOffsetY;
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
        auto mainNode = CocosUtils::playAnimation(CsbFile, 0, false);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        
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
    auto cell = static_cast<XTableViewCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = XTableViewCell::create();
    }
    
    for (int i = 0; i < columnCount; ++i) {
        const ssize_t index = idx * columnCount + i;
        auto node = dynamic_cast<BagNode*>(cell->getNode(i));
        if (index < 100 /* objects.count */) {
            const ObjectData* data = nullptr;
            if (node) {
                node->update(data, idx, i);
            } else {
                node = BagNode::create(data, idx, i);
                const Size& size = node->getContentSize();
                node->setPosition(Point(size.width * (i + 0.5f), size.height * 0.5f + nodeOffsetY));
                node->registerObserver(this);
                cell->addChild(node);
                cell->setNode(node, i);
            }
        } else if (node) {
            node->removeFromParent();
            cell->resetNode(i);
        }
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
        for (auto iter = _tabInfos.begin(); iter != _tabInfos.end(); ++iter) {
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
