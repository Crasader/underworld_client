//
//  PveInfoLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/17.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "PveInfoLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "ResourceNode.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

PveInfoLayer* PveInfoLayer::create(int levelId)
{
    PveInfoLayer *ret = new (nothrow) PveInfoLayer();
    if (ret && ret->init(levelId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

PveInfoLayer::PveInfoLayer()
:_observer(nullptr)
{
    
}

PveInfoLayer::~PveInfoLayer()
{
    removeAllChildren();
}

void PveInfoLayer::registerObserver(PveInfoLayerObserver *observer)
{
    _observer = observer;
}

bool PveInfoLayer::init(int levelId)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        
        static const string CsbFile("UI_Win.csb");
        Node* mainNode = CSLoader::createNode(CsbFile);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        
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
                                                Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_pveInfo_title"), DEFAULT_FONT_SIZE);
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
                                                Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_pveInfo_reward_label"), DEFAULT_FONT_SIZE);
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
                                                Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_pveInfo_rewardObject_label"), BIG_FONT_SIZE);
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
                            ResourceNode* rn = ResourceNode::create(kResourceType_Jade, 100);
                            child->addChild(rn);
                        }
                            break;
                        case 14:
                        {
                            ResourceNode* rn = ResourceNode::create(kResourceType_Gold, 100);
                            child->addChild(rn);
                        }
                            break;
                        case 16:
                        {
                            ResourceNode* rn = ResourceNode::create(kResourceType_Gem, 100);
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
                                        _observer->onPveInfoLayerContinued(this);
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
                                                    Label* label = CocosUtils::createLabel(LocalHelper::getString("battle_pveInfo_button_label"), DEFAULT_FONT_SIZE);
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
        eventListener->onTouchBegan = CC_CALLBACK_2(PveInfoLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(PveInfoLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool PveInfoLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void PveInfoLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
