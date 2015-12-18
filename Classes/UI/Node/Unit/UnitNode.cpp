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
#include "CocosGlobal.h"
#include "DataManager.h"
#include "UAConfigData.h"
#include "URConfigData.h"
#include "SoundManager.h"

using namespace std;
using namespace UnderWorld::Core;

static const float directionAngelEdge[UNIT_DIRECTIONS_COUNT] = {
    -30.f, 30.f, 90.f
};
static const int animationCheckerFrames(30);
static const float hpPercentageThreshold(50.0f);

#pragma mark ======================= inline unit getters =======================
static inline const string unit_getName(const Unit* unit)
{
    if (unit) {
            return unit->getUnitBase().getUnitName();
    }
    
    return "";
}

static inline bool unit_isMovable(const Unit* unit)
{
    return unit->getDefaultSkill(kSkillClass_Move) != nullptr;
}

static inline SkillClass unit_getSkillClass(const Unit* unit)
{
    return unit->getCurrentSkill()->getSkillType()->getSkillClass();
}

static bool hasStandbyAnimation(const Unit* unit)
{
    if (kSkillClass_Attack == unit_getSkillClass(unit)) {
        return true;
    }
    
    return false;
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
,_unit(nullptr)
,_configData(nullptr)
,_isBornOnTheRight(false)
,_needToFlip(false)
,_actionNode(nullptr)
,_currentAction(nullptr)
,_speedScheduler(nullptr)
,_actionManager(nullptr)
,_shadow(nullptr)
,_buf(nullptr)
,_hpBar(nullptr)
,_sprite(nullptr)
,_lastSkill(nullptr)
,_lastDirection(static_cast<UnitDirection>(-1))
,_lastHpPercentage(0.0f)
,_switchAnimationCounter(0)
,_isStandby(false)
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
    if (_observer) {
        _isBornOnTheRight = _observer->isUnitNodeBornOnTheRight(this);
        _needToFlip = (_isBornOnTheRight == _configData->isFaceRight());
    }
}

void UnitNode::update()
{
    if (_unit) {
        const Skill* currentSkill = _unit->getCurrentSkill();
        if (currentSkill) {
            bool needToUpdateUI(false);
            int currentFrame(0);
            UnitDirection direction(kUnitDirection_Left);
            bool flip(false);
            const float percentage(calculateHpPercentage());
            
            const SkillClass currentSkillClass(unit_getSkillClass(_unit));
            const UnitClass unitClass(_unit->getUnitBase().getUnitClass());
            
            switch (unitClass) {
                case kUnitClass_Warrior:
                case kUnitClass_Hero:
                {
                    calculateDirection(direction, flip);
                    if (_lastSkill) {
                        const SkillClass lastSkillClass(_lastSkill->getSkillType()->getSkillClass());
                        if (currentSkillClass != lastSkillClass) {
                            // TODO: remove temp code
                            if (hasStandbyAnimation(_unit) && _isStandby != checkIsStandby()) {
                                _isStandby = !_isStandby;
                            } else if (_isStandby) {
                                _isStandby = false;
                            }
                            needToUpdateUI = true;
                        } else if (_lastDirection != direction) {
                            switch (currentSkillClass) {
                                case kSkillClass_Move:
                                case kSkillClass_Attack:
                                {
                                    ++ _switchAnimationCounter;
                                    if (_switchAnimationCounter >= animationCheckerFrames) {
                                        // TODO: remove temp code
                                        if (hasStandbyAnimation(_unit) && _isStandby != checkIsStandby()) {
                                            _isStandby = !_isStandby;
                                        }
                                        
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
                        } else {
                            // TODO: remove temp code
                            // check if the unit is standby
                            if (hasStandbyAnimation(_unit) && _isStandby != checkIsStandby()) {
                                _isStandby = !_isStandby;
                                needToUpdateUI = true;
                            }
                        }
                    } else {
                        needToUpdateUI = true;
                    }
                }
                    break;
                case kUnitClass_Core:
                case kUnitClass_Building:
                {
                    if (_lastSkill) {
                        const SkillClass lastSkillClass(_lastSkill->getSkillType()->getSkillClass());
                        if (currentSkillClass != lastSkillClass) {
                            const string& unitName(unit_getName(_unit));
                            if (WOLF_TOWER == unitName || VAMPIRE_TOWER == unitName) {
                                if (kSkillClass_Attack == currentSkillClass || kSkillClass_Attack == lastSkillClass) {
                                    // attack
                                    needToUpdateUI = true;
                                } else if (kSkillClass_Die == currentSkillClass) {
                                    // destroyed
                                    needToUpdateUI = true;
                                }
                            } else if (WOLF_CORE == unitName || VAMPIRE_CORE == unitName) {
                                if (kSkillClass_Die == currentSkillClass) {
                                    // destroyed
                                    needToUpdateUI = true;
                                }
                            }
                            
                            if (WOLF_CORE == unitName || VAMPIRE_CORE == unitName) {
                                if (!needToUpdateUI && (hpPercentageThreshold - _lastHpPercentage) * (hpPercentageThreshold - percentage) <= 0) {
                                    // hurt
                                    needToUpdateUI = true;
                                }
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
                // reset
                _switchAnimationCounter = 0;
                _lastSkill = currentSkill;
                _lastDirection = direction;
                _lastHpPercentage = percentage;
                
//                CCLOG("--------------------- direction: %d ---------------------", direction);
                
                const string& file = getCsbFile(direction, percentage);
                updateActionNode(currentSkill, file, currentFrame, flip ? !_needToFlip : _needToFlip);
            }
        }
        
        updateHPBar();
    }
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
    const string& file = _configData->getSwordEffect();
    if (file.length() > 0) {
        Node* effect = addEffect(file);
        
        // TODO: update temp code
        if (false) {
            const float scaleX(effect->getScaleX());
            effect->setScaleX(-1 * scaleX);
        }
    }
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

void UnitNode::onHurt(const string& trigger)
{
    const URConfigData* data = DataManager::getInstance()->getURConfigData(trigger);
    if (data) {
        const string file = data->getHurtSound();
        if (file.length() > 0) {
            SoundManager::getInstance()->playSound(file);
        }
        
        addEffect(data->getHurtEffect());
    }
}

void UnitNode::onWin()
{
#if false
    const string& file = getStandbyCsbFile(calculateDirection(), calculateHpPercentage() > hpPercentageThreshold);
    addActionNode(file, true, true, 0.0f, 0, nullptr);
#endif
}

void UnitNode::onLose()
{
    onWin();
}

bool UnitNode::init(const Unit* unit)
{
    if (Node::init())
    {
        _unit = unit;
        const string& unitName(unit_getName(unit));
        _configData = DataManager::getInstance()->getURConfigData(unitName);
        assert(_configData);
        
        return true;
    }
    
    return false;
}

const string UnitNode::getCsbFile(UnitDirection direction, float hpPercentage)
{
    const string& prefix = _configData->getPrefix();
    
    string csbFile;
    const UnitClass unitClass = _unit->getUnitBase().getUnitClass();
    const SkillClass skillClass(unit_getSkillClass(_unit));
    switch (skillClass) {
        case kSkillClass_Stop:
        {
            csbFile = getStandbyCsbFile(direction, hpPercentage > hpPercentageThreshold);
        }
            break;
        case kSkillClass_Move:
        {
            switch (unitClass) {
                case kUnitClass_Warrior:
                case kUnitClass_Hero:
                {
                    csbFile = prefix + StringUtils::format("-run-%d.csb", direction);
                }
                    break;
                    
                default:
                    assert(false);
                    break;
            }
        }
            break;
        case kSkillClass_Attack:
        {
            switch (unitClass) {
                case kUnitClass_Core:
                case kUnitClass_Building:
                {
                    const bool healthy(hpPercentage > hpPercentageThreshold);
                    const string& attack = _configData->getBAttack();
                    if (attack.length() > 0) {
                        csbFile = attack;
                    } else {
                        csbFile = getStandbyCsbFile(direction, healthy);
                    }
                }
                    break;
                case kUnitClass_Warrior:
                case kUnitClass_Hero:
                {
                    if (_isStandby) {
                        csbFile = getStandbyCsbFile(direction, hpPercentage > hpPercentageThreshold);
                    } else {
                        csbFile = prefix + StringUtils::format("-attack-%d.csb", direction);
                    }
                }
                    break;
                default:
                    break;
            }
        }
            break;
        case kSkillClass_Cast:
        {
            // TODO
        }
            break;
        case kSkillClass_Die:
        {
            switch (unitClass) {
                case kUnitClass_Core:
                case kUnitClass_Building:
                {
                    csbFile = _configData->getBDestroyed();
                }
                    break;
                case kUnitClass_Warrior:
                case kUnitClass_Hero:
                {
                    csbFile = prefix + StringUtils::format("-dead-%d.csb", direction);
                }
                    break;
                default:
                    break;
            }
        }
            break;
        default:
            assert(false);
            break;
    }
    
    return csbFile;
}

const string UnitNode::getStandbyCsbFile(UnitDirection direction, bool isHealthy)
{
    string csbFile;
    const UnitClass unitClass = _unit->getUnitBase().getUnitClass();
    
    switch (unitClass) {
        case kUnitClass_Core:
        case kUnitClass_Building:
        {
            const string& normal = _configData->getBNormal();
            const string& damaged = _configData->getBDamaged();
            csbFile = isHealthy ? normal : (damaged.length() > 0 ? damaged : normal);
        }
            break;
        case kUnitClass_Warrior:
        case kUnitClass_Hero:
        {
            const string& prefix = _configData->getPrefix();
            csbFile = prefix + StringUtils::format("-standby-%d.csb", direction);
        }
            break;
        default:
            break;
    }
    
    return csbFile;
}

void UnitNode::getMultipleAnimationFiles(vector<string>& output)
{
    output.clear();
    if (kSkillClass_Attack == unit_getSkillClass(_unit)) {
        const string& attackBegin = _configData->getBAttackBegin();
        const string& attackEnd = _configData->getBAttackEnd();
        if (attackBegin.length() > 0 || attackEnd.length() > 0) {
            // 1. attack begin
            if (attackBegin.length() > 0) {
                output.push_back(attackBegin);
            }
            // 2. attack
            output.push_back(_configData->getBAttack());
            // 3. attack end
            if (attackEnd.length() > 0) {
                output.push_back(attackEnd);
            }
        }
    }
}

bool UnitNode::checkIsStandby()
{
    const SkillClass skillClass(unit_getSkillClass(_unit));
    if (kSkillClass_Attack == skillClass) {
        if (unit_isMovable(_unit)) {
            const Skill::SkillState state(_unit->getCurrentSkill()->getSkillState());
            if (Skill::SkillState::kSkillState_idle == state) {
                return true;
            }
        }
    } else if (kSkillClass_Stop == skillClass) {
        return true;
    }
    
    return false;
}

void UnitNode::calculateDirection(UnitDirection& direction, bool& flip)
{
    if (_unit) {
        const Coordinate& lastPos = _unit->getLastPos();
        if (lastPos.x == INVALID_VALUE && lastPos.y == INVALID_VALUE) {
            // new created
            direction = kUnitDirection_Left;
            flip = false;
        } else {
            const Coordinate& centerPos = _unit->getCenterPos();
            Coordinate currentPos(0, 0);
            Coordinate targetPos(0, 0);
            if (kSkillClass_Attack == unit_getSkillClass(_unit)) {
                currentPos = centerPos;
                targetPos = _unit->getTargetPos();
            } else {
                const UnitBase& unitBase = _unit->getUnitBase();
                currentPos = lastPos + Coordinate(unitBase.getSize() / 2, unitBase.getSize() / 2);
                targetPos = centerPos;
            }
            
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
            
//            CCLOG("==== angel : %.1f ====\n currentPos: %d-%d, targetPos: %d-%d", angel, currentPos.x, currentPos.y, targetPos.x, targetPos.y);
            direction = kUnitDirection_Left;
            for (int i = 0; i < UNIT_DIRECTIONS_COUNT; ++i) {
                if (angel < directionAngelEdge[i]) {
                    direction = static_cast<UnitDirection>(4 - i);
                    break;
                }
            }
            
            // flip if the unit walks rear
            flip = false;
            if (deltaX > 5 && _observer) {
                const bool dx = (targetPos.x - currentPos.x) >= 0;
                flip = (dx == _isBornOnTheRight);
            }
        }
    }
}

float UnitNode::calculateHpPercentage()
{
    const int maxHp = _unit->getUnitBase().getMaxHp();
    const int hp = _unit->getHp();
    return 100 * (float)hp / (float)maxHp;
}

void UnitNode::addActionNode(const string& file, bool play, bool loop, float playTime, int frameIndex, const function<void()>& lastFrameCallFunc)
{
    removeActionNode();
    
    if (file.length() > 0) {
        // add node
        _actionNode = CSLoader::createNode(file);
        _sprite = dynamic_cast<Sprite*>(*(_actionNode->getChildren().begin()));
        addChild(_actionNode);
        
        if (play) {
            // 1. add scheduler
            if (!_speedScheduler) {
                _speedScheduler = new Scheduler();
                Director::getInstance()->getScheduler()->scheduleUpdate(_speedScheduler, 0, false);
                if (!_actionManager) {
                    _actionManager = new ActionManager();
                    _speedScheduler->scheduleUpdate(_actionManager, 0, false);
                }
            }
            
            // 2. set actionManager every time before play animation
            if (_actionManager) {
                _actionNode->setActionManager(_actionManager);
            }
            
            // play animation if needed
            _currentAction = CSLoader::createTimeline(file);
            _actionNode->runAction(_currentAction);
            _currentAction->gotoFrameAndPlay(0, loop);
            _currentAction->setCurrentFrame(frameIndex);
            _currentAction->setLastFrameCallFunc(lastFrameCallFunc);
            
            const string& name = unit_getName(_unit);
            const SkillClass skillClass(unit_getSkillClass(_unit));

            AnimationParameters params = DataManager::getInstance()->getAnimationParameters(name, skillClass, _lastDirection);
            float scale = params.scale;
            float speed = params.speed;
            
            // set scale
            if (scale != 1.0f && scale > 0) {
                _actionNode->setScale(scale);
            }
            
            // the attack animation should be fit for preperforming time
            if (speed == 1.0f && playTime > 0.0f) {
                const float animationDuration((float)_currentAction->getDuration() / 60.0f);
                speed = animationDuration / playTime;
            }
            
            if (speed != 1.0f) {
                _speedScheduler->setTimeScale(speed);
            } else if (_speedScheduler->getTimeScale() != 1.0f) {
                _speedScheduler->setTimeScale(1.0f);
            }
        }
    }
}

void UnitNode::addMultipleAnimationNode(int frameIndex, const function<void()>& lastFrameCallFunc)
{
    static function<void()> callback = nullptr;
    callback = lastFrameCallFunc;
    
    const SkillClass skillClass = unit_getSkillClass(_unit);
    if (kSkillClass_Attack == skillClass) {
        if (_multipleAnimationFiles.size() > 0) {
            const string& file = _multipleAnimationFiles.front();
            addActionNode(file, true, false, 0.0f, 0, [this]() {
                _multipleAnimationFiles.erase(_multipleAnimationFiles.begin());
                addMultipleAnimationNode(0, callback);
            });
        } else if (callback) {
            callback();
        }
    }
}

void UnitNode::updateActionNode(const Skill* skill, const string& file, int currentFrame, bool flip)
{
    if (_unit) {
        removeActionNode();
        
        const UnitClass unitClass = _unit->getUnitBase().getUnitClass();
        const SkillClass skillClass(unit_getSkillClass(_unit));
        const bool isDead = (kSkillClass_Die == skillClass);
        
        // clear
        if (isDead) {
            removeShadow();
            removeBuf();
            removeHPBar();
        }
        
        getMultipleAnimationFiles(_multipleAnimationFiles);
        if (_multipleAnimationFiles.size() > 0) {
            addMultipleAnimationNode(0, nullptr);
        } else if (file.length() > 0) {
            const bool playAnimation(kUnitClass_Building != unitClass || (kSkillClass_Attack == skillClass || kSkillClass_Die == skillClass));
            const float playTime(hasStandbyAnimation(_unit) ? skill->getTotalPrePerformFrames() / (float)GameConstants::FRAME_PER_SEC : 0.0f);
            // add node
            addActionNode(file, playAnimation, !isDead, playTime, currentFrame, [=]() {
                if (isDead && _observer) {
                    _observer->onUnitNodePlayDeadAnimationFinished(this);
                }
            });
        } else {
            assert(false);
        }
        
        // flip if needed
        if (flip) {
            const float scaleX = _actionNode->getScaleX();
            _actionNode->setScaleX(-1 * scaleX);
        }
        
        if (isDead) {
            const string file = _configData->getDieSound();
            if (file.length() > 0) {
                SoundManager::getInstance()->playSound(file);
            }
            // TODO: remove irregular code
            setLocalZOrder(-1000);
        } else {
            if (kSkillClass_Attack == skillClass) {
                if (_currentAction) {
                    // if it is footman
                    if (_configData->isShortRange()) {
                        _currentAction->setFrameEventCallFunc([this](cocostudio::timeline::Frame* frame) {
                            playAttackSound();
                            if (_observer) {
                                _observer->onUnitNodeHurtTheTarget(this);
                            }
                        });
                    } else {
                        playAttackSound();
                    }
                }
            }
        }
        
        if (_sprite) {
            // add HP bar
            if (!isDead && !_hpBar) {
                addHPBar();
                updateHPBar();
            }
        }
        
    } else {
        assert(false);
    }
}

void UnitNode::removeActionNode()
{
    if (_actionNode) {
        _actionNode->stopAllActions();
        _actionNode->removeFromParent();
        _actionNode = nullptr;
        _currentAction = nullptr;
    }
}

void UnitNode::addHPBar()
{
    if (!_hpBar && _unit) {
        _hpBar = DisplayBar::create(kHP, _unit->getBelongFaction()->getFactionIndex(), _unit->getUnitBase().getUnitClass());
        const Size& size = _sprite->getContentSize();
        const Point& pos = _sprite->getPosition();
        const Point position(pos + Point(0, size.height / 2 + 10.0f));
        _hpBar->setPosition(convertToNodeSpace(_actionNode->convertToWorldSpace(position)));
        addChild(_hpBar);
    }
}

void UnitNode::updateHPBar()
{
    if (_hpBar && _unit) {
        _hpBar->setPercentage(calculateHpPercentage());
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

void UnitNode::playAttackSound()
{
    const string file = _configData->getAttackSound();
    if (file.length() > 0) {
        SoundManager::getInstance()->playSound(file);
    }
}