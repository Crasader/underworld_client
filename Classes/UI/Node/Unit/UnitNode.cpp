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
,_actionNode(nullptr)
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
    if (_unit) {
        const Skill* currentSkill = _unit->getCurrentSkill();
        if (currentSkill != _lastSkill) {
            _lastSkill = currentSkill;
            updateActionNode(_unit);
        }
    }
}

bool UnitNode::init(const Unit* unit)
{
    if (Node::init())
    {
        _unit = unit;
        
        updateActionNode(unit);
        
        return true;
    }
    
    return false;
}

void UnitNode::updateActionNode(const Unit* unit)
{
    if (unit) {
        if (_actionNode) {
            _actionNode->stopAllActions();
            _actionNode->removeFromParent();
        }
        
#if true
        static string csbFile("unit_0_1.csb");
#else
        UnitClass unitClass = unit->getUnitType()->getUnitClass();
        SkillClass skillClass = unit->getCurrentSkill()->getSkillType()->getSkillClass();
        string csbFile(StringUtils::format("unit_%d_%d.csb", unitClass, skillClass));
#endif
        
        _actionNode = CSLoader::createNode(csbFile);
        addChild(_actionNode);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        _actionNode->runAction(action);
        action->gotoFrameAndPlay(0, true);
    } else {
        // test
        string csbFile(StringUtils::format("unit_%d_%d.csb", 0, 1));
        _actionNode = CSLoader::createNode(csbFile);
        addChild(_actionNode);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        _actionNode->runAction(action);
        action->gotoFrameAndPlay(0, true);
    }
}