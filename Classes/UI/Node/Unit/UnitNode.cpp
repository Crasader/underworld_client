//
//  UnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UnitNode.h"
#include "cocostudio/CocoStudio.h"
#include "World.h"
#include "Faction.h"
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
,_shadow(nullptr)
,_unit(nullptr)
,_lastSkill(nullptr)
{
    
}

UnitNode::~UnitNode()
{
    removeAllChildren();
}

const Unit* UnitNode::getUnit() const
{
    return _unit;
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
        
//        UnitClass unitClass = unit->getUnitType()->getUnitClass();
        SkillClass skillClass = unit->getCurrentSkill()->getSkillType()->getSkillClass();
        const bool isDead = (kSkillClass_Die == skillClass);
        string csbFile;
        if (kSkillClass_Attack == skillClass) {
            csbFile = "play-wolf-attack.csb";
        } else if (isDead) {
            csbFile = "play-wolf-dead.csb";
        } else {
            csbFile = "play-wolf-run.csb";
        }
        
        _actionNode = CSLoader::createNode(csbFile);
        const int factionIndex = unit->getWorld()->getThisFactionIndex();
        if (unit->getBelongFaction()->getFactionIndex() != factionIndex) {
            const float scaleX = _actionNode->getScaleX();
            _actionNode->setScaleX(-1 * scaleX);
        }
        addChild(_actionNode);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        _actionNode->runAction(action);
        action->gotoFrameAndPlay(0, !isDead);
        if (isDead) {
            action->setLastFrameCallFunc([this]() {
                if (_observer) {
                    _observer->onUnitNodePlayDeadAnimationFinished(this);
                }
            });
        }
        
        // add shadow
        Sprite *sprite = dynamic_cast<Sprite*>(*(_actionNode->getChildren().begin()));
        if (sprite) {
            static const string shadowFile("GameImages/test/backcircle.png");
            const Size& size = sprite->getContentSize();
            _shadow = Sprite::create(shadowFile);
            _shadow->setPosition(sprite->getPosition() - Point(0, size.height * 0.25f));
            _actionNode->addChild(_shadow, -1);
        }
    } else {
        // test
        string csbFile(StringUtils::format("unit_%d_%d.csb", 0, 1));
        _actionNode = CSLoader::createNode(csbFile);
        _actionNode->setScale(0.5f);
        addChild(_actionNode);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        _actionNode->runAction(action);
        action->gotoFrameAndPlay(0, true);
    }
}