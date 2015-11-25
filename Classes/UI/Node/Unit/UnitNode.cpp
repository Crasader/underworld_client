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
#include "DisplayBar.h"

using namespace std;
using namespace UnderWorld::Core;

static const int directionCount = 5;
static const float directionAngelEdge[directionCount] = {
    -67.5f, -22.5f, 22.5f, 67.5f, 90.0f
};

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
,_lastDirection(kUnitDirection_Left)
,_hpBar(nullptr)
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
        const UnitClass unitClass = _unit->getUnitType()->getUnitClass();
        const Skill* currentSkill = _unit->getCurrentSkill();
        if (kUnitClass_Warrior == unitClass ||
            kUnitClass_Hero == unitClass) {
            UnitDirection direction = calculateDirection(_unit);
            if (currentSkill) {
                if (nullptr == _lastSkill) {
                    setCurrentSkill(currentSkill, direction);
                } else {
                    const SkillClass currentSkillClass = currentSkill->getSkillType()->getSkillClass();
                    const SkillClass lastSkillClass = _lastSkill->getSkillType()->getSkillClass();
                    if (currentSkillClass != lastSkillClass) {
                        setCurrentSkill(currentSkill, direction);
                    } else if (_lastDirection != direction) {
                        if (kSkillClass_Move == currentSkillClass ||
                            kSkillClass_Attack == currentSkillClass) {
                            setCurrentSkill(currentSkill, direction);
                        }
                    }
                }
            }
        } else if (kUnitClass_Building == unitClass) {
            if (currentSkill && nullptr == _lastSkill) {
                setCurrentSkill(currentSkill, kUnitDirection_Left);
            }
        } else if (kUnitClass_Core == unitClass) {
            if (currentSkill && nullptr == _lastSkill) {
                setCurrentSkill(currentSkill, kUnitDirection_Left);
            }
        }
        
        updateHPBar();
    }
}

bool UnitNode::init(const Unit* unit)
{
    if (Node::init())
    {
        _unit = unit;
        
        return true;
    }
    
    return false;
}

UnitNode::UnitDirection UnitNode::calculateDirection(const Unit* unit)
{
    if (unit) {
        const Coordinate& currentPos = unit->getPos();
        const Coordinate& targetPos = unit->getTargetPos();
        const float deltaX = abs(targetPos.x - currentPos.x);
        const float deltaY = targetPos.y - currentPos.y;
        
        float angel = 0.0f;
        if (deltaX == 0) {
            if (deltaY > 0) {
                angel = 90.0f;
            } else if (deltaY < 0) {
                angel = -90.0f;
            }
        } else {
            angel = 180.0f * atanf(deltaY / deltaX) / M_PI;
        }
        
        UnitNode::UnitDirection direction = kUnitDirection_Left;
        for (int i = 0; i < directionCount; ++i) {
            if (angel < directionAngelEdge[i]) {
                direction = static_cast<UnitNode::UnitDirection>(i + 1);
                break;
            }
        }
        
        return direction;
    }
    
    return kUnitDirection_Left;
}

void UnitNode::setCurrentSkill(const Skill* skill, UnitDirection direction)
{
    _lastSkill = skill;
    _lastDirection = direction;
    updateActionNode(_unit, direction);
}

void UnitNode::updateActionNode(const Unit* unit, UnitDirection direction)
{
    if (unit) {
        if (_actionNode) {
            _actionNode->stopAllActions();
            _actionNode->removeFromParent();
        }
        
        const UnitClass unitClass = unit->getUnitType()->getUnitClass();
        const bool movable = (kUnitClass_Warrior == unitClass || kUnitClass_Hero == unitClass);
        SkillClass skillClass = unit->getCurrentSkill()->getSkillType()->getSkillClass();
        const bool isDead = (kSkillClass_Die == skillClass);
        string csbFile;
        if (kSkillClass_Attack == skillClass) {
            if (movable) {
                csbFile = StringUtils::format("wolf-attack-%d.csb", direction);
            }
        } else if (isDead) {
            if (movable) {
                csbFile = StringUtils::format("wolf-dead-%d.csb", direction);
            }
            
            if (_hpBar) {
                _hpBar->removeFromParent();
                _hpBar = nullptr;
            }
        } else {
            if (movable) {
                csbFile = StringUtils::format("wolf-run-%d.csb", direction);
            } else if (kUnitClass_Core == unitClass) {
                csbFile = "effect-wolf-Base.csb";
            } else {
                // TODO: remove test code
                csbFile = "wolf-run-3.csb";
            }
        }
        
        if (csbFile.length() > 0) {
            _actionNode = CSLoader::createNode(csbFile);
            const int factionIndex = unit->getWorld()->getThisFactionIndex();
            if (unit->getBelongFaction()->getFactionIndex() == factionIndex) {
                const float scaleX = _actionNode->getScaleX();
                _actionNode->setScaleX(-1 * scaleX);
            }
            addChild(_actionNode);
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
            _actionNode->runAction(action);
            action->gotoFrameAndPlay(0, !isDead);
            if (isDead) {
                // TODO: remove irregular code
                setLocalZOrder(-1);
                action->setLastFrameCallFunc([this]() {
                    if (_observer) {
                        _observer->onUnitNodePlayDeadAnimationFinished(this);
                    }
                });
            }
            
            Sprite *sprite = dynamic_cast<Sprite*>(*(_actionNode->getChildren().begin()));
            if (sprite) {
                const Size& size = sprite->getContentSize();
                // add shadow
                if (kSkillClass_Move == skillClass ||
                    kSkillClass_Attack == skillClass) {
                    static const string shadowFile("GameImages/test/backcircle.png");
                    _shadow = Sprite::create(shadowFile);
                    _shadow->setPosition(sprite->getPosition() - Point(0, size.height * 0.25f));
                    _actionNode->addChild(_shadow, -1);
                }
                
                // add HP bar
                if (!isDead && !_hpBar) {
                    _hpBar = DisplayBar::create(kHP, unitClass);
                    const Point pos(sprite->getPosition() + Point(0, size.height / 2 + 10.0f));
                    _hpBar->setPosition(convertToNodeSpace(_actionNode->convertToWorldSpace(pos)));
                    addChild(_hpBar);
                    updateHPBar();
                }
            }
        } else {
            assert(false);
        }
    } else {
        // test
        
    }
}

void UnitNode::updateHPBar()
{
    if (_hpBar && _unit) {
        const int maxHp = _unit->getUnitType()->getMaxHp();
        const int hp = _unit->getHp();
        _hpBar->setPercentage(100 * (float)hp / (float)maxHp);
    }
}