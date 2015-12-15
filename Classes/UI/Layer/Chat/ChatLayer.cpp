//
//  ChatLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "ChatCell.h"
#include "ChatNode.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

static const float nodeOffsetX(17.0f);
static const float nodeOffsetY(17.0f);
static const int visibleCellsCount(6);

ChatLayer* ChatLayer::create(int levelId)
{
    ChatLayer *ret = new (nothrow) ChatLayer();
    if (ret && ret->init(levelId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatLayer::ChatLayer()
:_observer(nullptr)
,_scrollBar(nullptr)
,_tabIndex(-1)
,_editBoxBg(nullptr)
,_editBox(nullptr)
{
    static const Size unitNodeSize = ChatNode::create()->getContentSize();
    _cellSize.height = unitNodeSize.height + nodeOffsetY * 2;
    _cellSize.width = unitNodeSize.width + nodeOffsetX;
    
    _tableViewMaxSize.width = _cellSize.width * visibleCellsCount + nodeOffsetX;
    _tableViewMaxSize.height = _cellSize.height;
}

ChatLayer::~ChatLayer()
{
    removeAllChildren();
}

void ChatLayer::registerObserver(ChatLayerObserver *observer)
{
    _observer = observer;
}

bool ChatLayer::init(int levelId)
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
        
        addEditBox();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(ChatLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(ChatLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool ChatLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void ChatLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}

#pragma mark - TableViewDelegate
Size ChatLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return Size(618, 60);
}

TableViewCell* ChatLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
    ChatCell *cell = static_cast<ChatCell*>(table->dequeueCell());
    
    if (!cell) {
        cell = ChatCell::create();
    }
    
    ChatNode* node = cell->getNode();
    if (node) {
        
    } else {
        
    }
    
    return cell;
}

#pragma mark - EditBoxDelegate
void ChatLayer::editBoxTextChanged(ui::EditBox* editBox, const std::string& text)
{
    
}

void ChatLayer::editBoxReturn(ui::EditBox* editBox)
{
    
}

ssize_t ChatLayer::numberOfCellsInTableView(TableView *table)
{
    return 1;
}

#pragma mark - private
void ChatLayer::addEditBox()
{
    _editBox = ui::EditBox::create(_editBoxBg->getContentSize(), nullptr);
    addChild(_editBox);
    _editBox->setPosition(_editBoxBg->getPosition());
    _editBox->setDelegate(this);
    // configuration
    _editBox->setMaxLength(120);
    _editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    _editBox->setInputMode(ui::EditBox::InputMode::ANY);
    
    _editBox->setFontColor(Color4B::WHITE);
    _editBox->setPlaceHolder("Please input...");
}

void ChatLayer::switchTable(int index)
{
    if (_tabIndex != index) {
        _tabIndex = index;
        // TODO: refreshTable
        
        setButtonSelected(_tabIndex);
    }
}

void ChatLayer::setButtonSelected(int index)
{
    static string normalFile = "GameImages/world/ui_fenye_4.png";
    static string selectedFile = "GameImages/world/ui_fenye_3.png";
    
    Button *selectedButton = _tabButtons.at(index);
    set<Button *> otherButtons;
    for (int i = 0; i < _tabButtons.size(); ++i) {
        otherButtons.insert(_tabButtons.at(i));
    }
    otherButtons.erase(selectedButton);
    // 1. set all buttons' ZOrder to 1
    const Vector<Node*>& children = selectedButton->getParent()->getChildren();
    for (auto child : children)
    {
        child->setLocalZOrder(1);
    }
    // 2. make sure the selected button is on the top
    selectedButton->loadTextures(selectedFile, selectedFile);
    selectedButton->setLocalZOrder(2);
    // 3. make sure the other buttons are on the bottom
    for (set<Button *>::iterator iter = otherButtons.begin(); iter != otherButtons.end(); ++iter)
    {
        (*iter)->loadTextures(normalFile, normalFile);
        (*iter)->setLocalZOrder(0);
    }
}
