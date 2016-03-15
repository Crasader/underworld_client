//
//  BattleSmallResourceNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/27.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleSmallResourceNode.h"
#include "CocosUtils.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

BattleSmallResourceNode::BattleSmallResourceNode()
:_type(ResourceType::Gold)
,_count(0)
,_icon(nullptr)
,_countLabel(nullptr)
{
    
}

BattleSmallResourceNode::~BattleSmallResourceNode()
{
    removeFromParent();
}

BattleSmallResourceNode* BattleSmallResourceNode::create(ResourceType type, int count)
{
    BattleSmallResourceNode *p = new (nothrow) BattleSmallResourceNode();
    if(p && p->init(type, count))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool BattleSmallResourceNode::init(ResourceType type, int count)
{
    if(Node::init())
    {
        _type = type;
        _count = count;
        
        string file;
        if (ResourceType::Gold == type) {
            file = "UI-quan-2.csb";
        } else if (ResourceType::Wood == type) {
            file = "UI-quan-1.csb";
        }
        Node *effect = CSLoader::createNode(file);
        addChild(effect);
        _icon = effect;
        
        timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, true);
        
        const Size size(effect->getChildren().front()->getContentSize());
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        effect->setPosition(Point(size.width / 2, size.height / 2));
        
        _countLabel = CocosUtils::createLabel(StringUtils::format("%d", count), DEFAULT_FONT_SIZE, DEFAULT_NUMBER_FONT);
        _countLabel->setPosition(effect->getPosition());
        addChild(_countLabel, 1);
        
        return true;
    }
    
    return false;
}

void BattleSmallResourceNode::check(float count)
{
    if (_countLabel) {
        const Color4B& color = (count >= _count) ? ENABLED_COLOR : DISABLED_COLOR;
        if (_countLabel->getTextColor() != color) {
            _countLabel->setTextColor(color);
        }
    }
}

void BattleSmallResourceNode::setCount(int count)
{
    if (count != _count) {
        _count = count;
        
        if (_countLabel) {
            _countLabel->setString(StringUtils::format("%d", count));
        }
    }
}

ResourceType BattleSmallResourceNode::getType() const
{
    return _type;
}

int BattleSmallResourceNode::getCount() const
{
    return _count;
}
