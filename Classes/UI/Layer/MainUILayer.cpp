//
//  MainUILayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/9.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MainUILayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

MainUILayer* MainUILayer::create()
{
    MainUILayer *ret = new (nothrow) MainUILayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MainUILayer::MainUILayer()
:_observer(nullptr)
,_iconButton(nullptr)
,_nameLabel(nullptr)
,_levelLabel(nullptr)
,_expProgressBar(nullptr)
,_boltLabel(nullptr)
,_coinLabel(nullptr)
,_gemLabel(nullptr)
{
    
}

MainUILayer::~MainUILayer()
{
    removeAllChildren();
}

void MainUILayer::registerObserver(MainUILayerObserver *observer)
{
    _observer = observer;
}

bool MainUILayer::init()
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        static const float margin(5.0f);
        
        // 1. left top
        {
            static const string csbFile("guanggaomianban.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(margin, winSize.height - margin));
            addChild(mainNode);
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
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
                            case 101:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->setPressedActionEnabled(true);
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonCancelSound();
                                        // TODO:
                                    });
                                }
                                _iconButton = button;
                            }
                                break;
                            case 100:
                            {
                                Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                                child->addChild(label);
                                _nameLabel = label;
                            }
                                break;
                            case 97:
                            {
                                LabelAtlas* label = CocosUtils::create10x25Number("");
                                child->addChild(label);
                                _levelLabel = label;
                            }
                                break;
                            case 103:
                            {
                                LoadingBar* bar = dynamic_cast<LoadingBar*>(child);
                                bar->setScale9Enabled(true);
                                bar->setCapInsets(Rect(3, 3, 341, 30));
                                _expProgressBar = bar;
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        // 2. left
        {
            static const string csbFile("guanggaomianban.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(margin, winSize.height / 2));
            addChild(mainNode);
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
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
                            case 101:
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
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        // 3. left bottom
        {
            static const string csbFile("guanggaomianban.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(margin, margin));
            addChild(mainNode);
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
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
                            case 101:
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
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        // 4. right top
        {
            static const string csbFile("guanggaomianban.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(winSize.width - margin, winSize.height - margin));
            addChild(mainNode);
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
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
                            case 101:
                            {
                                LabelAtlas* label = CocosUtils::create12x30Number(StringUtils::format("%d", 1));
                                label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                                child->addChild(label);
                                _boltLabel = label;
                            }
                                break;
                            case 100:
                            {
                                LabelAtlas* label = CocosUtils::create12x30Number(StringUtils::format("%d", 1));
                                label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                                child->addChild(label);
                                _coinLabel = label;
                            }
                                break;
                            case 97:
                            {
                                LabelAtlas* label = CocosUtils::create12x30Number(StringUtils::format("%d", 1));
                                label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                                child->addChild(label);
                                _gemLabel = label;
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        // 5. right bottom
        {
            static const string csbFile("guanggaomianban.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(winSize.width - margin, margin));
            addChild(mainNode);
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
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
                            case 101:
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
                            case 97:
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
                            case 103:
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
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MainUILayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MainUILayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool MainUILayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return false;
}

void MainUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
