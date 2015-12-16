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
#include "ResourceNode.h"
#include "BattleScene.h"

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
,_pvpButton(nullptr)
,_bagButton(nullptr)
,_questButton(nullptr)
,_optionButton(nullptr)
,_guildButton(nullptr)
,_armyButton(nullptr)
,_jadeResourceNode(nullptr)
,_goldResourceNode(nullptr)
,_gemResourceNode(nullptr)
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
            static const string csbFile("UI_CharInfo.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(margin, winSize.height - margin));
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
                            case 15:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                                _iconButton = button;
                            }
                                break;
                            case 8:
                            {
                                Label* label = CocosUtils::createLabel("123", DEFAULT_FONT_SIZE);
                                child->addChild(label);
                                _nameLabel = label;
                            }
                                break;
                            case 11:
                            {
                                LabelAtlas* label = CocosUtils::create10x25Number("&'.56");
                                child->addChild(label);
                                _levelLabel = label;
                            }
                                break;
                            case 10:
                            {
                                LoadingBar* bar = dynamic_cast<LoadingBar*>(child);
                                bar->setScale9Enabled(true);
                                bar->setCapInsets(Rect(1, 1, 176, 2));
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
            static const string csbFile("UI_ChatIcon.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(0, winSize.height / 2));
            addChild(mainNode);
            
            Node* root = mainNode;
            const Vector<Node*>& children = root->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                Node* child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 20:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->setPressedActionEnabled(true);
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
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
            static const string csbFile("UI_PVPICON.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(margin, margin));
            addChild(mainNode);
            
            Node* root = mainNode->getChildByTag(23);
            const Vector<Node*>& children = root->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                Node* child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 24:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    updateButtonTextures(button, 25, "icon_pvp_1", "icon_pvp_2");
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        CocosUtils::replaceScene(BattleScene::create(1));
                                    });
                                }
                                
                                _pvpButton = button;
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
            ResourceNode* node = ResourceNode::create(kResourceType_Gem, 100);
            const Size& size = node->getContentSize();
            static float offsetX(20);
            static float offsetY(20);
            const float x = winSize.width - offsetX - size.width / 2;
            const float y = winSize.height - offsetY - size.height / 2;
            
            node->setPosition(Point(x, y));
            addChild(node);
            _gemResourceNode = node;
            
            node = ResourceNode::create(kResourceType_Gold, 100);
            node->setPosition(Point(x - (offsetX + size.width), y));
            addChild(node);
            _goldResourceNode = node;
            
            node = ResourceNode::create(kResourceType_Jade, 100);
            node->setPosition(Point(x - (offsetX + size.width) * 2, y));
            addChild(node);
            _jadeResourceNode = node;
        }
        
        // 5. right bottom
        {
            static const string csbFile("UI_FunctionIcon.csb");
            Node *mainNode = CSLoader::createNode(csbFile);
            mainNode->setPosition(Point(winSize.width - margin, margin));
            addChild(mainNode);
            
            Node* root = mainNode->getChildByTag(21);
            const Vector<Node*>& children = root->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                Node* child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 22:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    updateButtonTextures(button, 23, "icon_renwu_1", "icon_renwu_2");
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                                
                                _questButton = button;
                            }
                                break;
                            case 100:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                            }
                                break;
                            case 97:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                            }
                                break;
                            case 103:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                            }
                                break;
                            case 104:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
                                        // TODO:
                                    });
                                }
                            }
                                break;
                            case 105:
                            {
                                Button* button = dynamic_cast<Button*>(child);
                                if (button) {
                                    button->addClickEventListener([this](Ref *pSender){
                                        SoundManager::getInstance()->playButtonSound();
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
        
//        updateIcon();
        updateResources();
        
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

void MainUILayer::updateButtonTextures(Button* button, int childTag, const string& normal, const string& touched)
{
    Node* node = button->getChildByTag(childTag);
    if (node) {
        static const string prefix("GameImages/icons/");
        static const string suffix(".png");
        static const int textureTag(1000);
        
        const string normalFile = prefix + normal + suffix;
        const string touchedFile = prefix + touched + suffix;
        
        Sprite* s = Sprite::create(normalFile);
        s->setTag(textureTag);
        node->addChild(s);
        
        button->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type) {
            if (type == Widget::TouchEventType::BEGAN) {
                node->removeChildByTag(textureTag);
                Sprite* s = Sprite::create(touchedFile);
                s->setTag(textureTag);
                node->addChild(s);
            } else if (type == Widget::TouchEventType::CANCELED || type == Widget::TouchEventType::ENDED) {
                node->removeChildByTag(textureTag);
                Sprite* s = Sprite::create(normalFile);
                s->setTag(textureTag);
                node->addChild(s);
            }
        });
    }
}

void MainUILayer::setButtonSelected(Button* button)
{
    
}

void MainUILayer::updateIcon()
{
    const string file("GameImages/avatars/icon_user.png");
    _iconButton->loadTextures(file, file);
}

void MainUILayer::updateResources()
{
    
}