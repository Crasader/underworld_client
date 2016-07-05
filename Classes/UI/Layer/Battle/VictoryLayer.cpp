//
//  VictoryLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "VictoryLayer.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "ResourceNode.h"

using namespace std;
using namespace ui;

VictoryLayer* VictoryLayer::create(int levelId)
{
    VictoryLayer *ret = new (nothrow) VictoryLayer();
    if (ret && ret->init(levelId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

VictoryLayer::VictoryLayer()
:_observer(nullptr)
{
    
}

VictoryLayer::~VictoryLayer()
{
    removeAllChildren();
}

void VictoryLayer::registerObserver(VictoryLayerObserver *observer)
{
    _observer = observer;
}

bool VictoryLayer::init(int levelId)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        
        static const string CsbFile("UI_Win.csb");
        Node* mainNode = CocosUtils::playAnimation(CsbFile, 0, false);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        
        Node* root = mainNode->getChildByTag(6);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                        case 9:
                        {
                            Button* button = dynamic_cast<Button*>(child);
                            if (button) {
                                button->setPressedActionEnabled(true);
                                button->addClickEventListener([this](Ref *pSender){
                                    SoundManager::getInstance()->playButtonCancelSound();
                                    removeFromParent();
                                });
                            }
                        }
                            break;
                        case 7:
                        {
                            const Vector<Node*>& children = child->getChildren();
                            for (int i = 0; i < children.size(); ++i)
                            {
                                Node* child = children.at(i);
                                if (child) {
                                    const int tag = child->getTag();
                                    if (tag > 0) {
                                        switch (tag) {
                                            case 8:
                                            {
                                                Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_victory_title"), DEFAULT_FONT_SIZE);
                                                child->addChild(label);
                                            }
                                                break;
                                                
                                            default:
                                                break;
                                        }
                                    }
                                }
                            }
                        }
                            break;
                        case 23:
                        {
                            const Vector<Node*>& children = child->getChildren();
                            for (int i = 0; i < children.size(); ++i)
                            {
                                Node* child = children.at(i);
                                if (child) {
                                    const int tag = child->getTag();
                                    if (tag > 0) {
                                        switch (tag) {
                                            case 24:
                                            {
                                                Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_victory_reward_label"), DEFAULT_FONT_SIZE);
                                                child->addChild(label);
                                            }
                                                break;
                                                
                                            default:
                                                break;
                                        }
                                    }
                                }
                            }
                        }
                            break;
                        case 13:
                        {
                            Sprite* bg = Sprite::create(StringUtils::format("GameImages/level_bg/level_%d.png", levelId));
                            child->addChild(bg, -1);
                            
                            const Vector<Node*>& children = child->getChildren();
                            for (int i = 0; i < children.size(); ++i)
                            {
                                Node* child = children.at(i);
                                if (child) {
                                    const int tag = child->getTag();
                                    if (tag > 0) {
                                        switch (tag) {
                                            case 12:
                                            {
                                                Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_victory_hint"), BIG_FONT_SIZE);
                                                child->addChild(label);
                                            }
                                                break;
                                                
                                            default:
                                                break;
                                        }
                                    }
                                }
                            }
                        }
                            break;
                        case 15:
                        {
                            ResourceNode* rn = ResourceNode::create(ResourceType::Jade, 100);
                            child->addChild(rn);
                        }
                            break;
                        case 14:
                        {
                            ResourceNode* rn = ResourceNode::create(ResourceType::Gold, 100);
                            child->addChild(rn);
                        }
                            break;
                        case 16:
                        {
                            ResourceNode* rn = ResourceNode::create(ResourceType::Gem, 100);
                            child->addChild(rn);
                        }
                            break;
                        case 17:
                        {
                            Button* button = dynamic_cast<Button*>(child);
                            if (button) {
                                button->setPressedActionEnabled(true);
                                button->addClickEventListener([this](Ref *pSender){
                                    SoundManager::getInstance()->playButtonGoOnSound();
                                    if (_observer) {
                                        _observer->onVictoryLayerContinued(this);
                                    }
                                });
                                
                                const Vector<Node*>& children = child->getChildren();
                                for (int i = 0; i < children.size(); ++i)
                                {
                                    Node* child = children.at(i);
                                    if (child) {
                                        const int tag = child->getTag();
                                        if (tag > 0) {
                                            switch (tag) {
                                                case 18:
                                                {
                                                    Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_victory_button_label"), DEFAULT_FONT_SIZE);
                                                    child->addChild(label);
                                                }
                                                    break;
                                                    
                                                default:
                                                    break;
                                            }
                                        }
                                    }
                                }
                            }
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
        eventListener->onTouchBegan = CC_CALLBACK_2(VictoryLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(VictoryLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool VictoryLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void VictoryLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}