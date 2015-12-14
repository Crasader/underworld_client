//
//  ChatNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatNode.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

ChatNode* ChatNode::create()
{
    ChatNode *ret = new (nothrow) ChatNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatNode::ChatNode()
:_observer(nullptr)
{
    
}

ChatNode::~ChatNode()
{
    removeAllChildren();
}

void ChatNode::registerObserver(ChatNodeObserver *observer)
{
    _observer = observer;
}

bool ChatNode::init()
{
    if (Node::init())
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
        
        return true;
    }
    
    return false;
}
