//
//  UnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UnitNode.h"
#include "cocostudio/CocoStudio.h"
#include "Unit.h"
#include "UnitType.h"
#include "Skill.h"
#include "SkllType.h"

using namespace std;
using namespace UnderWorld::Core;

UnitNode* UnitNode::create(const Unit* unit)
{
    UnitNode *ret = new (nothrow) UnitNode();
    if (ret && ret->init(unit))
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

UnitNode::UnitNode()
:_observer(nullptr)
,_unit(nullptr)
,_lastSkill(nullptr)
{
    
}

UnitNode::~UnitNode()
{
    removeAllChildren();
}

void UnitNode::registerObserver(UnitNodeObserver *observer)
{
    _observer = observer;
}

void UnitNode::update()
{
    const Skill* currentSkill = _unit->getCurrentSkill();
    if (currentSkill != _lastSkill) {
        _lastSkill = currentSkill;
        // TODO: switch Skill animation
    }
}

bool UnitNode::init(const Unit* unit)
{
    if (Node::init())
    {
        _unit = unit;
        
        const UnitType* type = unit->getUnitType();
        
        string csbFile = (type->getUnitClass() == kUnitClass_Warrior) ? "battle_heroRed_1.csb" : "battle_herogreen_1.csb";
        Node *mainNode = CSLoader::createNode(csbFile);
        addChild(mainNode);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        
        return true;
    }
    
    return false;
}