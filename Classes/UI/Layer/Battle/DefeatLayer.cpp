//
//  DefeatLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "DefeatLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

DefeatLayer* DefeatLayer::create(int levelId)
{
    DefeatLayer *ret = new (nothrow) DefeatLayer();
    if (ret && ret->init(levelId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

DefeatLayer::DefeatLayer()
:_observer(nullptr)
{
    
}

DefeatLayer::~DefeatLayer()
{
    removeAllChildren();
}

void DefeatLayer::registerObserver(DefeatLayerObserver *observer)
{
    _observer = observer;
}

bool DefeatLayer::init(int levelId)
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
        eventListener->onTouchBegan = CC_CALLBACK_2(DefeatLayer::onTouchBegan, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(DefeatLayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool DefeatLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    return true;
}

void DefeatLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    
}
