//
//  QuestLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "QuestCell.h"
#include "QuestNode.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

static const float nodeOffsetX(17.0f);
static const float nodeOffsetY(17.0f);
static const int visibleCellsCount(6);

QuestLayer* QuestLayer::create(int levelId)
{
    QuestLayer *ret = new (nothrow) QuestLayer();
    if (ret && ret->init(levelId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

QuestLayer::QuestLayer()
:_observer(nullptr)
,_scrollBar(nullptr)
,_tabIndex(-1)
{
    static const Size& unitNodeSize = QuestNode::create()->getContentSize();
    _cellSize.height = unitNodeSize.height + nodeOffsetY * 2;
    _cellSize.width = unitNodeSize.width + nodeOffsetX;
    
    _tableViewMaxSize.width = _cellSize.width * visibleCellsCount + nodeOffsetX;
    _tableViewMaxSize.height = _cellSize.height;
}

QuestLayer::~QuestLayer()
{
    removeAllChildren();
}

void QuestLayer::registerObserver(QuestLayerObserver *observer)
{
    _observer = observer;
}

bool QuestLayer::init(int levelId)
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
                            Button* button = dynamic_cast<Button*>(child);
                            if (button) {
                                button->setPressedActionEnabled(true);
                                button->addClickEventListener([this](Ref *pSender){
                                    SoundManager::getInstance()->playButtonCancelSound();
                                    // TODO:
                                });
                            }
                        }
                            break;
                        case 101:
                        {
                            LabelAtlas* label = CocosUtils::create12x30Number(StringUtils::format("%d", 1));
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                            child->addChild(label);
                        }
                            break;
                        case 102:
                        {
                            LabelAtlas* label = CocosUtils::create12x30Number(StringUtils::format("%d", 1));
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                            child->addChild(label);
                        }
                            break;
                        case 103:
                        {
                            LabelAtlas* label = CocosUtils::create12x30Number(StringUtils::format("%d", 1));
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                            child->addChild(label);
                        }
                            break;
                        case 104:
                        {
                            Button* button = dynamic_cast<Button*>(child);
                            if (button) {
                                button->setPressedActionEnabled(true);
                                button->addClickEventListener([this](Ref *pSender){
                                    SoundManager::getInstance()->playButtonCancelSound();
                                    // TODO:
                                });
                            }
                        }
                            break;
                        case 105:
                        {
                            Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                            child->addChild(label);
                        }
                            break;
                        case 106:
                        {
                            Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                            child->addChild(label);
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(QuestLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(QuestLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool QuestLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void QuestLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDelegate
Size QuestLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return Size(618, 60);
}

TableViewCell* QuestLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    QuestCell *cell = static_cast<QuestCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = QuestCell::create();
    }
    
    QuestNode* node = cell->getNode();
    if (node) {
        
    } else {
        
    }
    
    return cell;
}

ssize_t QuestLayer::numberOfCellsInTableView(TableView *table)
{
    return 1;
}

void QuestLayer::switchTable(int index)
{
    
}

void QuestLayer::setButtonSelected(int index)
{
    
}

void QuestLayer::refreshTable()
{
    
}