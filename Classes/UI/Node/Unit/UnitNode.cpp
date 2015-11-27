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

static const int directionCount = 3;
static const float directionAngelEdge[directionCount] = {
    -30.f, 30.f, 90.f
};
static const float hpPercentageThreshold(50.0f);

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
,_buf(nullptr)
,_hpBar(nullptr)
,_sprite(nullptr)
,_unit(nullptr)
,_lastSkill(nullptr)
,_lastDirection(kUnitDirection_Left)
,_lastHpPercentage(0)
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
        if (currentSkill) {
            bool needToUpdateUI(false);
            UnitDirection direction(kUnitDirection_Left);
            const int hp(_unit->getHp());
            const int maxHp(_unit->getUnitType()->getMaxHp());
            const float percentage(100 * (float)hp / (float)maxHp);
            
            const UnitClass unitClass = _unit->getUnitType()->getUnitClass();
            // warriors or heroes
            if (kUnitClass_Warrior == unitClass ||
                kUnitClass_Hero == unitClass) {
                direction = calculateDirection(_unit);
                if (_lastSkill) {
                    const SkillClass currentSkillClass = currentSkill->getSkillType()->getSkillClass();
                    const SkillClass lastSkillClass = _lastSkill->getSkillType()->getSkillClass();
                    if (currentSkillClass != lastSkillClass) {
                        needToUpdateUI = true;
                    } else if (_lastDirection != direction) {
                        if (kSkillClass_Move == currentSkillClass ||
                            kSkillClass_Attack == currentSkillClass) {
                            needToUpdateUI = true;
                        }
                    }
                } else {
                    needToUpdateUI = true;
                }
            }
            // buildings or cores
            else if (kUnitClass_Building == unitClass) {
                if (_lastSkill) {
//                    setCurrentSkill(currentSkill, direction);
                } else {
                    needToUpdateUI = true;
                }
            } else if (kUnitClass_Core == unitClass) {
                if (_lastSkill) {
                    if ((hpPercentageThreshold - _lastHpPercentage) * (hpPercentageThreshold - percentage) < 0) {
                        needToUpdateUI = true;
                    }
                } else {
                    needToUpdateUI = true;
                }
            }
            
            if (needToUpdateUI) {
                _lastSkill = currentSkill;
                _lastDirection = direction;
                _lastHpPercentage = percentage;
                updateActionNode(_unit, direction, percentage);
            }
        }
        
        updateHPBar();
    }
}

void UnitNode::addCritEffect()
{
    addEffect("Strike point-3.csb");
}

void UnitNode::addBlockEffect()
{
    Node* effect = addEffect("effect-Block.csb");
    effect->setScale(1.5f);
}

void UnitNode::addRecoveryEffect()
{
    Node* effect = addEffect("effect-recovery.csb");
    effect->setLocalZOrder(-1);
}

void UnitNode::addBuf()
{
    if (!_buf) {
        static const string file("effect-buff-2.csb");
        const Size& size = _sprite->getContentSize();
        const Point& pos = _sprite->getPosition();
        _buf = CSLoader::createNode(file);
        _buf->setPosition(pos - Point(0, size.height * 0.25f));
        addChild(_buf, -1);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        _buf->runAction(action);
        action->gotoFrameAndPlay(0, true);
    }
}

void UnitNode::removeBuf()
{
    if (_buf) {
        _buf->removeFromParent();
        _buf = nullptr;
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
#if true
        const Coordinate& currentPos = unit->getCenterPos();
        const Coordinate& targetPos = unit->getTargetPos();
#else
        const bool isAttacking = (kSkillClass_Attack == unit->getCurrentSkill()->getSkillType()->getSkillClass()) ? true : false;
        const UnitType* unitType = unit->getUnitType();
        const Coordinate& centerPos = unit->getCenterPos();
        const Coordinate& currentPos = isAttacking ? centerPos : (unit->getLastPos() + Coordinate(unitType->getSize() / 2, unitType->getSize() / 2));
        const Coordinate& targetPos = isAttacking ? unit->getTargetPos() : centerPos;
#endif
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
        
        CCLOG("==== angel : %.1f ====\n currentPos: %d-%d, targetPos: %d-%d", angel, currentPos.x, currentPos.y, targetPos.x, targetPos.y);
        
        UnitNode::UnitDirection direction = kUnitDirection_Left;
        for (int i = 0; i < directionCount; ++i) {
            if (angel < directionAngelEdge[i]) {
                direction = static_cast<UnitNode::UnitDirection>(i + 2);
                break;
            }
        }
        
        CCLOG("---- direction : %d ----", direction);
        return direction;
    }
    
    return kUnitDirection_Left;
}

void UnitNode::updateActionNode(const Unit* unit, UnitDirection direction, float hpPercentage)
{
    if (unit) {
        if (_actionNode) {
            _actionNode->stopAllActions();
            _actionNode->removeFromParent();
            _actionNode = nullptr;
        }
        
        const UnitClass unitClass = unit->getUnitType()->getUnitClass();
        const bool isMovableUnit = (kUnitClass_Warrior == unitClass || kUnitClass_Hero == unitClass);
        
        SkillClass skillClass = unit->getCurrentSkill()->getSkillType()->getSkillClass();
        const bool isDead = (kSkillClass_Die == skillClass);
        
        // remove
        if (isDead) {
            removeShadow();
            removeBuf();
        }
        
        const bool isOpponent(unit->getBelongFaction()->getFactionIndex() != unit->getWorld()->getThisFactionIndex());
        string csbFile;
        
        // attack
        if (kSkillClass_Attack == skillClass) {
            if (isMovableUnit) {
                csbFile = StringUtils::format("wolf-attack-%d.csb", direction);
            } else if (kUnitClass_Building == unitClass) {
                csbFile = "wolf-tower defense.csb";
            }
        }
        // die
        else if (isDead) {
            if (isMovableUnit) {
                csbFile = StringUtils::format("wolf-dead-%d.csb", direction);
            }
            
            if (_hpBar) {
                _hpBar->removeFromParent();
                _hpBar = nullptr;
            }
        }
        // move
        else if (kSkillClass_Move == skillClass) {
            if (isMovableUnit) {
                csbFile = StringUtils::format("wolf-run-%d.csb", direction);
            }
        }
        // stop
        else if (kSkillClass_Stop == skillClass) {
            if (isMovableUnit) {
                csbFile = "wolf-play-Standby.csb";
            } else if (kUnitClass_Core == unitClass) {
                if (hpPercentage > hpPercentageThreshold) {
                    csbFile = isOpponent ? "effect-Vampire-base.csb" : "effect-wolf-Base_1.csb";
                } else {
                    if (hpPercentage <= 0.0f) {
                        csbFile = isOpponent ? "effect-Vampire-base-Severe damage.csb" : "effect-wolf-base-Severe damage.csb";
                    } else {
                        csbFile = isOpponent ? "effect-Vampire-base-damage.csb" : "effect-wolf-base-damage.csb";
                    }
                }
            } else if (kUnitClass_Building == unitClass) {
                csbFile = "wolf-tower defense.csb";
            }
        } else {
            // TODO: remove test code
            csbFile = "wolf-run-3.csb";
        }
        
        if (csbFile.length() > 0) {
            _actionNode = CSLoader::createNode(csbFile);
            // flip if needed
            if (!isOpponent) {
                if (kUnitClass_Core == unitClass) {
                    _actionNode->setScale(0.6f);
                } else if (kUnitClass_Building == unitClass) {
                    
                } else {
                    const float scaleX = _actionNode->getScaleX();
                    _actionNode->setScaleX(-1 * scaleX);
                }
            } else if (kUnitClass_Building == unitClass) {
                const float scaleX = _actionNode->getScaleX();
                _actionNode->setScaleX(-1 * scaleX);
            }
            
            // TODO: remove test code
            if (kUnitClass_Core == unitClass) {
                _actionNode->setScale(0.6f);
            }
            
            addChild(_actionNode);
            
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
            // TODO: remove temp code
            if (kUnitClass_Building != unitClass || kSkillClass_Attack == skillClass) {
                _actionNode->runAction(action);
                action->gotoFrameAndPlay(0, !isDead);
            }
            
            if (isDead) {
                // TODO: remove irregular code
                setLocalZOrder(-1);
                // if it is a warrior or a hero
                if (isMovableUnit) {
                    action->setLastFrameCallFunc([this]() {
                        if (_observer) {
                            _observer->onUnitNodePlayDeadAnimationFinished(this);
                        }
                    });
                }
            } else {
                if (kSkillClass_Attack == skillClass) {
                    const AttackSkillType* asType = dynamic_cast<const AttackSkillType*>(_unit->getUnitType()->getDefaultAttackSkillType(kFieldType_Land));
                    // if it is footman
                    if (asType) {
                        if (asType->getRange() < 5) {
                            action->setFrameEventCallFunc([this](cocostudio::timeline::Frame* frame) {
                                const unsigned int frameIndex = frame->getFrameIndex();
                                if (10 == frameIndex) {
                                    if (_observer) {
                                        _observer->onUnitNodeFootmanAttackedTheTarget(this);
                                    }
                                }
                            });
                        }
                    }
                }
            }
            
            _sprite = dynamic_cast<Sprite*>(*(_actionNode->getChildren().begin()));
            if (_sprite) {
                const Size& size = _sprite->getContentSize();
                const Point& pos = _sprite->getPosition();
                // add shadow
                if (kSkillClass_Move == skillClass ||
                    kSkillClass_Attack == skillClass) {
                    addShadow();
                    addBuf();
                }
                
                // add HP bar
                if (!isDead && !_hpBar) {
                    _hpBar = DisplayBar::create(kHP, unitClass);
                    const Point position(pos + Point(0, size.height / 2 + 10.0f));
                    _hpBar->setPosition(convertToNodeSpace(_actionNode->convertToWorldSpace(position)));
                    addChild(_hpBar);
                    updateHPBar();
                }
            }
        } else {
            assert(false);
        }
    } else {
        assert(false);
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

void UnitNode::addShadow()
{
    if (!_shadow) {
        static const string file("GameImages/test/backcircle.png");
        const Size& size = _sprite->getContentSize();
        const Point& pos = _sprite->getPosition();
        _shadow = Sprite::create(file);
        _shadow->setPosition(pos - Point(0, size.height * 0.25f));
        addChild(_shadow, -1);
    }
}

void UnitNode::removeShadow()
{
    if (_shadow) {
        _shadow->removeFromParent();
        _shadow = nullptr;
    }
}

Node* UnitNode::addEffect(const string& file)
{
    if (_sprite) {
        Node *effect = CSLoader::createNode(file);
        effect->setPosition(_sprite->getPosition());
        addChild(effect);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, false);
        action->setLastFrameCallFunc([effect]() {
            effect->removeFromParent();
        });
        
        return effect;
    }
    
    return nullptr;
}