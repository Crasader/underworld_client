//
//  UnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UnitNode.h"
#include "World.h"
#include "Faction.h"
#include "Unit.h"
#include "UnitType.h"
#include "Skill.h"
#include "SkllType.h"
#include "DisplayBar.h"

using namespace std;
using namespace UnderWorld::Core;

static const int directionCount(3);
static const float directionAngelEdge[directionCount] = {
    -30.f, 30.f, 90.f
};
static const int animationCheckerFrames(30);
static const float hpPercentageThreshold(50.0f);

#pragma mark ======================= inline unit getters =======================
static inline bool unit_isThisFaction(const Unit* unit)
{
    return unit->getBelongFaction()->getFactionIndex() == unit->getWorld()->getThisFactionIndex();
}

static inline bool unit_isShortRange(const Unit* unit)
{
    const AttackSkillType* asType = dynamic_cast<const AttackSkillType*>(unit->getUnitType()->getDefaultAttackSkillType(kFieldType_Land));
    return (asType && asType->getRange() < 5) ? true : false;
}

static inline bool unit_isWizard(const Unit* unit)
{
    const string& name = unit->getUnitType()->getName();
    const string suffix("巫师");
    return (name.find(suffix) != string::npos);
}

static inline bool unit_isMovable(const Unit* unit)
{
    const UnitClass unitClass = unit->getUnitType()->getUnitClass();
    return (kUnitClass_Warrior == unitClass || kUnitClass_Hero == unitClass) ? true : false;
}

static inline SkillClass unit_getSkillClass(const Unit* unit)
{
    return unit->getCurrentSkill()->getSkillType()->getSkillClass();
}

#pragma mark ======================= Class UnitNode =======================
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
,_currentAction(nullptr)
,_speedScheduler(nullptr)
,_actionManager(nullptr)
,_shadow(nullptr)
,_buf(nullptr)
,_hpBar(nullptr)
,_sprite(nullptr)
,_unit(nullptr)
,_lastSkill(nullptr)
,_lastDirection(static_cast<UnitDirection>(-1))
,_lastHpPercentage(0.0f)
,_switchAnimationCounter(0)
{
    
}

UnitNode::~UnitNode()
{
    if (_speedScheduler) {
        Director::getInstance()->getScheduler()->unscheduleUpdate(_speedScheduler);
        if (_actionManager) {
            _speedScheduler->unscheduleUpdate(_actionManager);
        }
        CC_SAFE_RELEASE(_speedScheduler);
    }
    
    if (_actionManager) {
        CC_SAFE_RELEASE(_actionManager);
    }
    
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
            int currentFrame(0);
            UnitDirection direction(kUnitDirection_Left);
            const int hp(_unit->getHp());
            const int maxHp(_unit->getUnitType()->getMaxHp());
            const float percentage(100 * (float)hp / (float)maxHp);
            
            const SkillClass currentSkillClass(currentSkill->getSkillType()->getSkillClass());
            const UnitClass unitClass(_unit->getUnitType()->getUnitClass());
            
            switch (unitClass) {
                case kUnitClass_Warrior:
                case kUnitClass_Hero:
                {
                    direction = calculateDirection();
                    if (_lastSkill) {
                        const SkillClass lastSkillClass(_lastSkill->getSkillType()->getSkillClass());
                        if (currentSkillClass != lastSkillClass) {
                            needToUpdateUI = true;
                        } else if (_lastDirection != direction) {
                            switch (currentSkillClass) {
                                case kSkillClass_Move:
                                case kSkillClass_Attack:
                                {
                                    ++ _switchAnimationCounter;
                                    if (_switchAnimationCounter >= animationCheckerFrames) {
                                        needToUpdateUI = true;
                                        if (_currentAction) {
                                            currentFrame = _currentAction->getCurrentFrame();
                                        }
                                    }
                                }
                                    break;
                                    
                                default:
                                    break;
                            }
                        }
                    } else {
                        needToUpdateUI = true;
                    }
                }
                    break;
                case kUnitClass_Building:
                case kUnitClass_Core:
                {
                    if (_lastSkill) {
                        const SkillClass lastSkillClass(_lastSkill->getSkillType()->getSkillClass());
                        if (currentSkillClass != lastSkillClass && (kSkillClass_Attack == currentSkillClass || kSkillClass_Attack == lastSkillClass)) {
                            // attack
                            if (kUnitClass_Core != unitClass) {
                                needToUpdateUI = true;
                            }
                        } else if (percentage <= 0.0f) {
                            // destroyed
                            needToUpdateUI = true;
                        } else if ((hpPercentageThreshold - _lastHpPercentage) * (hpPercentageThreshold - percentage) <= 0) {
                            // hurt
                            // TODO: remove test code
                            if (kUnitClass_Building != unitClass) {
                                needToUpdateUI = true;
                            }
                        }
                    } else {
                        needToUpdateUI = true;
                    }
                }
                    break;
                    
                default:
                    break;
            }
            
            if (needToUpdateUI) {
                updateActionNode(currentSkill, direction, percentage, currentFrame);
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

void UnitNode::addSwordEffect()
{
    addEffect("wolf-crit-daoguang.csb");
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

const string UnitNode::getCsbFile(UnitDirection direction, float hpPercentage, bool& flip)
{
    flip = false;
    
    const bool isWerewolf(unit_isThisFaction(_unit));
    const bool isShortRange(unit_isShortRange(_unit));
    const bool isMovableUnit = (unit_isMovable(_unit));
    const bool isWizard(unit_isWizard(_unit));
    
    string prefix;
    if (isMovableUnit) {
        if (isWerewolf) {
            if (isShortRange) {
                prefix = "wolf";
            } else {
                if (isWizard) {
                    prefix = "wolf-wizard";
                    flip = !flip;
                } else {
                    prefix = "wolf-Archer";
                    flip = !flip;
                }
                
                direction = kUnitDirection_Left;
            }
        } else {
            if (isShortRange) {
                prefix = "Vampire-tank";
            } else {
                if (isWizard) {
                    prefix = "Dead witch";
                } else {
                    prefix = "Dead-Archer";
                }
            }
            
            direction = kUnitDirection_Left;
        }
    }
    
    string csbFile;
    const UnitClass unitClass = _unit->getUnitType()->getUnitClass();
    const SkillClass skillClass(unit_getSkillClass(_unit));
    switch (skillClass) {
        case kSkillClass_Stop:
        {
            if (isMovableUnit) {
                if (isWerewolf) {
                    csbFile = "wolf-play-Standby.csb";
                } else {
                    csbFile = "wolf-play-Standby.csb";
                }
            } else if (kUnitClass_Core == unitClass) {
                if (hpPercentage > hpPercentageThreshold) {
                    csbFile = isWerewolf ? "effect-wolf-Base_1.csb" : "effect-Vampire-base.csb";
                } else {
                    csbFile = isWerewolf ? "effect-wolf-base-damage.csb" : "effect-Vampire-base-damage.csb";
                }
            } else if (kUnitClass_Building == unitClass) {
                csbFile = "wolf-tower defense.csb";
                if (!isWerewolf) {
                    flip = !flip;
                }
            }
        }
            break;
        case kSkillClass_Move:
        {
            if (isMovableUnit) {
                csbFile = prefix + StringUtils::format("-run-%d.csb", direction);
            } else {
                assert(false);
            }
        }
            break;
        case kSkillClass_Attack:
        {
            if (isMovableUnit) {
                csbFile = prefix + StringUtils::format("-attack-%d.csb", direction);
            } else if (kUnitClass_Building == unitClass) {
                csbFile = "wolf-tower defense.csb";
                if (!isWerewolf) {
                    flip = !flip;
                }
            }
        }
            break;
        case kSkillClass_Cast:
        {
            
        }
            break;
        case kSkillClass_Die:
        {
            if (isMovableUnit) {
                csbFile = prefix + StringUtils::format("-dead-%d.csb", direction);
            } else if (kUnitClass_Core == unitClass) {
                csbFile = isWerewolf ? "effect-wolf-base-Severe damage.csb" : "effect-Vampire-base-Severe damage.csb";
            } else if (kUnitClass_Building == unitClass) {
                csbFile = "wolf-tower defense.csb";
                if (!isWerewolf) {
                    flip = !flip;
                }
            }
        }
            break;
            
        default:
        {
            csbFile = "wolf-run-3.csb";
        }
            break;
    }
    
    return csbFile;
}

UnitNode::UnitDirection UnitNode::calculateDirection()
{
    if (_unit) {
#if false
        const Coordinate& currentPos = unit->getCenterPos();
        const Coordinate& targetPos = unit->getTargetPos();
#else
        const bool isAttacking = (kSkillClass_Attack == unit_getSkillClass(_unit)) ? true : false;
        const UnitType* unitType = _unit->getUnitType();
        const Coordinate& centerPos = _unit->getCenterPos();
        const Coordinate& currentPos = isAttacking ? centerPos : (_unit->getLastPos() + Coordinate(unitType->getSize() / 2, unitType->getSize() / 2));
        const Coordinate& targetPos = isAttacking ? _unit->getTargetPos() : centerPos;
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

void UnitNode::updateActionNode(const Skill* skill, UnitDirection direction, float hpPercentage, int currentFrame)
{
    // set
    _switchAnimationCounter = 0;
    _lastSkill = skill;
    _lastDirection = direction;
    _lastHpPercentage = hpPercentage;
    
    if (_unit) {
        if (_actionNode) {
            _actionNode->stopAllActions();
            _actionNode->removeFromParent();
            _actionNode = nullptr;
            _currentAction = nullptr;
        }
        
        const UnitClass unitClass = _unit->getUnitType()->getUnitClass();
        const bool isMovableUnit(unit_isMovable(_unit));
        const SkillClass skillClass(unit_getSkillClass(_unit));
        const bool isDead = (kSkillClass_Die == skillClass);
        
        // clear
        if (isDead) {
            removeShadow();
            removeBuf();
            removeHPBar();
        }
        
        bool flip(false);
        const string& csbFile = getCsbFile( direction, hpPercentage, flip);
        
        if (csbFile.length() > 0) {
            // add node
            _actionNode = CSLoader::createNode(csbFile);
            addChild(_actionNode);
            if (_actionManager) {
                _actionNode->setActionManager(_actionManager);
            }
            
            // TODO: remove temp code
            if (kUnitClass_Core == unitClass) {
                _actionNode->setScale(0.6f);
            }
            
            // get parameters
            float nodeScale;
            float animationSpeed;
            getActionParameters(nodeScale, animationSpeed);
            if (nodeScale != 1.0f) {
                const float scale = _actionNode->getScale();
                _actionNode->setScale(scale * nodeScale);
            }
            
            // flip if needed
            if (flip) {
                const float scaleX = _actionNode->getScaleX();
                _actionNode->setScaleX(-1 * scaleX);
            }
            
            // TODO: remove temp code
            if (kUnitClass_Building != unitClass || kSkillClass_Attack == skillClass) {
                // play animation if needed
                _currentAction = CSLoader::createTimeline(csbFile);
                _actionNode->runAction(_currentAction);
                _currentAction->gotoFrameAndPlay(0, !isDead);
                _currentAction->setCurrentFrame(currentFrame);
                
                // the attack animation should be fit for preperforming time
                if (false && kSkillClass_Attack == skillClass) {
                    const float preperformingTime((float)skill->getTotalPrePerformFrames() / (float)GameConstants::FRAME_PER_SEC);
                    const float animationDuration((float)_currentAction->getDuration() / 60.0f);
                    animationSpeed = animationDuration / preperformingTime;
                }
                
                if (animationSpeed != 1.0f) {
                    // add scheduler
                    if (!_speedScheduler) {
                        _speedScheduler = new Scheduler();
                        Director::getInstance()->getScheduler()->scheduleUpdate(_speedScheduler, 0, false);
                        if (!_actionManager) {
                            _actionManager = new ActionManager();
                            _speedScheduler->scheduleUpdate(_actionManager, 0, false);
                        }
                    }
                    
                    _speedScheduler->setTimeScale(animationSpeed);
                }
            }
            
            if (isDead) {
                // TODO: remove irregular code
                setLocalZOrder(-1);
                // if it is a warrior or a hero
                if (isMovableUnit && _currentAction) {
                    _currentAction->setLastFrameCallFunc([this]() {
                        if (_observer) {
                            _observer->onUnitNodePlayDeadAnimationFinished(this);
                        }
                    });
                }
            } else {
                if (kSkillClass_Attack == skillClass) {
                    if (_currentAction) {
                        // if it is footman
                        if (unit_isShortRange(_unit)) {
                            _currentAction->setFrameEventCallFunc([this](cocostudio::timeline::Frame* frame) {
                                if (_observer) {
                                    _observer->onUnitNodeFootmanAttackedTheTarget(this);
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

void UnitNode::removeHPBar()
{
    if (_hpBar) {
        _hpBar->removeFromParent();
        _hpBar = nullptr;
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

void UnitNode::getActionParameters(float& actionNodeScale, float& animationSpeed) const
{
    actionNodeScale = 1.0f;
    animationSpeed = 1.0f;
    
    if (_unit) {
        if (!(unit_isMovable(_unit))) {
            return;
        }
        
        const bool isWerewolf(unit_isThisFaction(_unit));
        const SkillClass skillClass(unit_getSkillClass(_unit));
        const bool isShortRange(unit_isShortRange(_unit));
        const bool isWizard(unit_isWizard(_unit));
        if (isWerewolf) {
            // werewolf
            switch (skillClass) {
                case kSkillClass_Stop:
                {
                    if (isShortRange) {
                        // footman
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Move:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Attack:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Cast:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Die:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                default:
                    break;
            }
        } else {
            // vampire
            switch (skillClass) {
                case kSkillClass_Stop:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Move:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Attack:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Cast:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                case kSkillClass_Die:
                {
                    if (isShortRange) {
                        // footman
                        
                    } else if (isWizard) {
                        // wizard
                        
                    } else {
                        // archer
                        
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }
}