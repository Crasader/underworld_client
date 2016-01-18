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
#include "UnitType.h"
#include "Skill.h"
#include "SkllType.h"
#include "Spell.h"
#include "DisplayBar.h"
#include "CocosGlobal.h"
#include "DataManager.h"
#include "UAConfigData.h"
#include "URConfigData.h"
#include "SpellConfigData.h"
#include "EffectConfigData.h"
#include "SoundManager.h"
#include "CocosUtils.h"

using namespace std;
using namespace UnderWorld::Core;

#pragma mark ======================= static =======================
static const float hpPercentageThreshold(50.0f);

static inline SkillClass unit_getSkillClass(const Unit* unit)
{
    return unit->getCurrentSkill()->getSkillType()->getSkillClass();
}

static void node_flipX(Node* node)
{
    const float scaleX = node->getScaleX();
    node->setScaleX(-1 * scaleX);
}

static void node_setScale(Node* node, float scale)
{
    const float scaleX = node->getScaleX();
    const float scaleY = node->getScaleY();
    node->setScale(scale * scaleX, scale * scaleY);
}

static int getResourceId(Unit::Direction direction)
{
    switch (direction) {
        case Unit::kDirection_right:
        case Unit::kDirection_left:
            return 3;
        case Unit::kDirection_rightUp:
        case Unit::kDirection_leftUp:
            return 2;
        case Unit::kDirectoin_rightDown:
        case Unit::kDirection_leftDown:
            return 4;
            
        default:
            assert(false);
            return 0;
    }
}

#pragma mark ======================= Class UnitNode =======================
UnitNode* UnitNode::create(const Unit* unit, bool rightSide)
{
    UnitNode *ret = new (nothrow) UnitNode(unit, rightSide);
    if (ret && ret->init())
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

UnitNode::UnitNode(const Unit* unit, bool rightSide)
// base
:_observer(nullptr)
,_unit(unit)
,_isBuilding(false)
,_configData(nullptr)
,_needToFlip(false)
// animations used
,_actionNode(nullptr)
,_currentAction(nullptr)
,_speedScheduler(nullptr)
,_actionManager(nullptr)
,_shadow(nullptr)
,_hpBar(nullptr)
,_sprite(nullptr)
,_idLabel(nullptr)
,_lastSkill(nullptr)
,_lastDirection(static_cast<Unit::Direction>(-1))
,_isLastHealthy(true)
,_isLastFlipped(false)
,_isStandby(false)
,_isPlayingAttackAnimation(false)
,_animationCounter(0)
{
    _unitName = unit->getUnitBase().getRenderKey();
    
    UnitClass uc = unit->getUnitBase().getUnitClass();
    _isBuilding = (kUnitClass_Core == uc || kUnitClass_Building == uc);
    _configData = DataManager::getInstance()->getURConfigData(_unitName);
    _needToFlip = (rightSide == _configData->isFaceRight());
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
            // direction and flip
            Unit::Direction direction = _unit->getDirection();
            bool flip = needToFlip(direction);
            
            //
            bool needToUpdateUI(false);
            int currentFrame(0);
            const float percentage(calculateHpPercentage());
            const bool isHealthy = (percentage > hpPercentageThreshold);
            
            if (_lastSkill) {
                const SkillClass currentSkillClass(unit_getSkillClass(_unit));
                const SkillClass lastSkillClass(_lastSkill->getSkillType()->getSkillClass());
                const UnitClass uc = _unit->getUnitBase().getUnitClass();
                
                if (kUnitClass_Core == uc) {
                    if (currentSkillClass != lastSkillClass && kSkillClass_Die == currentSkillClass) {
                        // destroyed
                        needToUpdateUI = true;
                    } else if (isHealthy != _isLastHealthy) {
                        // hurt
                        needToUpdateUI = true;
                    }
                } else {
                    if (currentSkillClass != lastSkillClass) {
                        // check if the unit is standby
                        if (needToChangeStandbyStatus()) {
                            _isStandby = !_isStandby;
                        } else if (_isStandby) {
                            _isStandby = false;
                        }
                        
                        needToUpdateUI = true;
                    }
                    else if (_lastDirection != direction || _isLastFlipped != flip) {
                        // check if the unit is standby
                        if (needToChangeStandbyStatus()) {
                            _isStandby = !_isStandby;
                        }
                        
                        needToUpdateUI = true;
                        if (_currentAction) {
                            currentFrame = _currentAction->getCurrentFrame();
                        }
                    } else {
                        // check if the unit is standby
                        if (needToChangeStandbyStatus()) {
                            _isStandby = !_isStandby;
                            needToUpdateUI = true;
                        }
                    }
                }
            } else {
                needToUpdateUI = true;
            }
            
            if (needToUpdateUI) {
//                CCLOG("--------------------- direction: %d ---------------------", direction);
                
                // reset
                _lastSkill = currentSkill;
                _lastDirection = direction;
                _isLastHealthy = isHealthy;
                _isLastFlipped = flip;
                
                reset();
                
                getCsbFiles(_animationFiles, direction, isHealthy);
                updateActionNode(currentSkill, currentFrame, flip);
            }
        }
        
        updateHPBar();
        updateBufs();
        updateFeatures();
        if (!_idLabel) {
//            _idLabel = CocosUtils::createLabel(StringUtils::format("%d", _unit->getUnitId()), 120.f);
//            this->addChild(_idLabel, 9999);
            
        }
    }
}

void UnitNode::addBlockEffect()
{
    Node* effect = addEffect("effect-Block.csb");
    node_setScale(effect, 1.5f);
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
        addEffect(file);
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

void UnitNode::onDead()
{
    removeShadow();
    removeAllBufs();
    removeHPBar();
    
    const string file = _configData->getDieSound();
    if (file.length() > 0) {
        SoundManager::getInstance()->playSound(file);
    }
    
    // TODO: remove irregular code
    setLocalZOrder(-1000);
}

void UnitNode::onWin()
{
    
}

void UnitNode::onLose()
{
    onWin();
}

bool UnitNode::init()
{
    if (Node::init())
    {
        return true;
    }
    
    return false;
}

void UnitNode::setOpacity(GLubyte opacity)
{
    Node::setOpacity(opacity);
    if (_sprite) {
        _sprite->setOpacity(opacity);
    }
}

GLubyte UnitNode::getOpacity() const
{
    if (_sprite) {
        return _sprite->getOpacity();
    }
    
    return Node::getOpacity();
}

#pragma mark - getters
void UnitNode::getCsbFiles(vector<string>& output, Unit::Direction direction, bool isHealthy)
{
    output.clear();
    
    const string& prefix = _configData->getPrefix();
    string csbFile;
    const SkillClass skillClass(unit_getSkillClass(_unit));
    switch (skillClass) {
        case kSkillClass_Stop:
        {
            csbFile = getStandbyCsbFile(direction, isHealthy);
        }
            break;
        case kSkillClass_Move:
        {
            if (_isBuilding) {
                assert(false);
            } else {
                csbFile = prefix + StringUtils::format("-run-%d.csb", getResourceId(direction));
            }
        }
            break;
        case kSkillClass_Attack:
        {
            if (_isStandby) {
                csbFile = getStandbyCsbFile(direction, isHealthy);
            } else {
                getAttackCsbFiles(output, direction, isHealthy);
            }
        }
            break;
        case kSkillClass_Cast:
        {
            // TODO
            if (_isStandby) {
                csbFile = getStandbyCsbFile(direction, isHealthy);
            } else {
                getAttackCsbFiles(output, direction, isHealthy);
            }
        }
            break;
        case kSkillClass_Die:
        {
            if (_isBuilding) {
                csbFile = _configData->getBDestroyed();
            } else {
                csbFile = prefix + StringUtils::format("-dead-%d.csb", getResourceId(direction));
            }
        }
            break;
        default:
            assert(false);
            break;
    }
    
    if (output.size() == 0 && csbFile.length() > 0) {
        output.push_back(csbFile);
    }
}

const string UnitNode::getStandbyCsbFile(Unit::Direction direction, bool isHealthy)
{
    string csbFile;
    if (_isBuilding) {
        const string& normal = StringUtils::format(_configData->getBNormal().c_str(), getResourceId(direction));
        const string& damaged = StringUtils::format(_configData->getBDamaged().c_str(), getResourceId(direction));
        csbFile = isHealthy ? normal : (damaged.length() > 0 ? damaged : normal);
    } else {
        const string& prefix = _configData->getPrefix();
        csbFile = prefix + StringUtils::format("-standby-%d.csb", getResourceId(direction));
    }
    
    return csbFile;
}

void UnitNode::getAttackCsbFiles(vector<string>& output, Unit::Direction direction, bool isHealthy)
{
    output.clear();
    if (_isBuilding) {
        // 1. attack begin
        const string& attackBegin = _configData->getBAttackBegin();
        if (attackBegin.length() > 0) {
            output.push_back(attackBegin);
        }
        // 2. attack
        const string& attack = StringUtils::format(_configData->getBAttack().c_str(), getResourceId(direction));
        if (attack.length() > 0) {
            output.push_back(attack);
        } else {
            output.push_back(getStandbyCsbFile(direction, isHealthy));
        }
        // 3. attack end
        const string& attackEnd = _configData->getBAttackEnd();
        if (attackEnd.length() > 0) {
            output.push_back(attackEnd);
        }
    } else {
        const string& prefix = _configData->getPrefix();
        {
            const string& attack = prefix + StringUtils::format("-attack-%d.csb", getResourceId(direction));
            output.push_back(attack);
        }
        {
            const string& backSing = prefix + StringUtils::format("-attack-%d-1.csb", getResourceId(direction));
            output.push_back(backSing);
        }
    }
}

bool UnitNode::needToChangeStandbyStatus()
{
    bool isStandby(false);
    if (kSkillClass_Attack == unit_getSkillClass(_unit)) {
        // check if the unit is standby
        const Skill::SkillState state(_unit->getCurrentSkill()->getSkillState());
        if (!_isPlayingAttackAnimation && Skill::SkillState::kSkillState_idle == state) {
            isStandby = true;
        }
        
        if (_isStandby != isStandby) {
            return true;
        }
    }
    
    return false;
}

bool UnitNode::needToFlip(Unit::Direction direction)
{
    bool flip(false);
    if (_unit) {
        const bool isFaceRight = _configData->isFaceRight();
        switch (direction) {
            case Unit::kDirection_right:
            case Unit::kDirection_rightUp:
            case Unit::kDirectoin_rightDown:
            {
                flip = !isFaceRight;
            }
                break;
            case Unit::kDirection_left:
            case Unit::kDirection_leftUp:
            case Unit::kDirection_leftDown:
            {
                flip = isFaceRight;
            }
                break;
                
            default:
                break;
        }
    }
    
    return flip;
}

float UnitNode::calculateHpPercentage()
{
    const int maxHp = _unit->getUnitBase().getMaxHp();
    const int hp = _unit->getHp();
    return 100 * (float)hp / (float)maxHp;
}

#pragma mark - add animation
void UnitNode::addActionNode(const string& file, bool play, bool loop, float playTime, int frameIndex, bool flip, const function<void()>& lastFrameCallFunc)
{
    removeActionNode();
    
    if (file.length() > 0) {
        // add node
        _actionNode = CSLoader::createNode(file);
        _sprite = dynamic_cast<Sprite*>(*(_actionNode->getChildren().begin()));
        addChild(_actionNode);
        
        // flip if needed
        if (flip) {
            node_flipX(_actionNode);
        }
        
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
            
            const SkillClass skillClass(unit_getSkillClass(_unit));

            AnimationParameters params = DataManager::getInstance()->getAnimationParameters(_unitName, skillClass, _lastDirection);
            float scale = params.scale;
            float speed = params.speed;
            
            // set scale
            if (scale != 1.0f && scale > 0) {
                node_setScale(_actionNode, scale);
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

void UnitNode::addStandbyActionNode()
{
    Unit::Direction direction = _unit->getDirection();
    bool flip = needToFlip(direction);
    const string& file = getStandbyCsbFile(direction, calculateHpPercentage() > hpPercentageThreshold);
    addActionNode(file, true, true, 0.0f, 0, flip, nullptr);
}

void UnitNode::addNextAttackActionNode(float playTime, int frameIndex)
{
    if (_animationFiles.empty()) {
        _isPlayingAttackAnimation = false;
        addStandbyActionNode();
    } else {
        const string& file = _animationFiles.front();
        addActionNode(file, true, false, playTime, frameIndex, _isLastFlipped, CC_CALLBACK_0(UnitNode::onAttackAnimationFinished, this));
    }
}

void UnitNode::onAttackAnimationFinished()
{
    // it means the "functionalIndex" of the animation sequence has a callback,
    // the others are only the animations without any function.
    static const int functionalIndex = 0;
    
    // 1. execute callback
    if (_animationCounter == functionalIndex) {
        // play sound
        const string file = _configData->getAttackSound();
        if (file.length() > 0) {
            SoundManager::getInstance()->playSound(file);
        }
        
        // if it is footman
        if (_configData->isShortRange()) {
            if (_observer) {
                _observer->onUnitNodeHurtTheTarget(this);
            }
        }
    }
    
    // 2. play the next animation
    _animationFiles.erase(_animationFiles.begin());
    ++ _animationCounter;
    addNextAttackActionNode(0.0f, 0);
    // because only the "functionalIndex" of the animation sequence has some function,
    // in order to avoid getting the incorrect "frameIndex", set "_currentAction" to null.
    if (_animationCounter != functionalIndex) {
        _currentAction = nullptr;
    }
}

void UnitNode::reset()
{
    _isPlayingAttackAnimation = false;
    _animationCounter = 0;
    _animationFiles.clear();
}

void UnitNode::updateActionNode(const Skill* skill, int frameIndex, bool flip)
{
    const ssize_t cnt = _animationFiles.size();
    if (_unit && cnt > 0) {
        // 1. remove the old action node
        removeActionNode();
        
        const SkillClass skillClass(unit_getSkillClass(_unit));
        const bool isDead = (kSkillClass_Die == skillClass);
        
        // 2. remove effects
        if (isDead) {
            onDead();
        }
        
        // 3. add the new action node
        {
            // attack
            if (kSkillClass_Attack == skillClass && !_isStandby) {
                _isPlayingAttackAnimation = true;
                const float playTime = skill->getTotalPrePerformFrames() / (float)GameConstants::FRAME_PER_SEC;
                addNextAttackActionNode(playTime, frameIndex);
            } else {
                const string& file = _animationFiles.front();
                // die
                if (isDead) {
                    // the unit might has been destroyed when animation finished,
                    // so save the unitId before playing the animation
                    const int unitId = _unit->getUnitId();
                    addActionNode(file, true, false, 0.0f, frameIndex, flip, [=]() {
                        if (_observer) {
                            _observer->onUnitNodePlayDeadAnimationFinished(unitId);
                        }
                    });
                } else {
                    // run / standby / cast
                    const bool playAnimation(kUnitClass_Building != _unit->getUnitBase().getUnitClass());
                    addActionNode(file, playAnimation, true, 0.0f, frameIndex, flip, nullptr);
                    
                    if (kSkillClass_Cast == skillClass) {
                        const Spell* spell = dynamic_cast<const Spell*>(_unit->getCurrentSkill());
                        if (spell) {
                            const string& spellName = spell->getSpellType()->getAlias();
                            const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(spellName);
                            if (data) {
                                const vector<string>& resources = data->getCasterResourceNames();
                                const size_t cnt = resources.size();
                                switch (cnt) {
                                    case 1:
                                    {
                                        addEffect(resources.at(0));
                                    }
                                        break;
                                    case 2:
                                    {
                                        addEffect(resources.at(0));
                                        Node* second = addEffect(resources.at(1));
                                        second->setLocalZOrder(-1);
                                        
                                    }
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // 4. add new effects
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

#pragma mark - effects
void UnitNode::updateBufs()
{
    set<string> last = _bufNames;
    
    _bufNames.clear();
    for (Unit::BuffIter iter = _unit->getBuffsBegin(); iter != _unit->getBuffsEnd(); ++iter)
    {
        const string& bufName = iter->second->getBuffType()->getRenderKey();
        if (bufName.length() > 0) {
            _bufNames.insert(bufName);
        }
    }
    
    set<string> intersections;
    set_intersection(last.begin(), last.end(), _bufNames.begin(), _bufNames.end(), inserter(intersections, intersections.begin()));
    
    set<string> added;
    set_difference(_bufNames.begin(), _bufNames.end(), intersections.begin(), intersections.end(), inserter(added, added.begin()));
    
    set<string> removed;
    set_difference(last.begin(), last.end(), intersections.begin(), intersections.end(), inserter(removed, removed.begin()));
    
    for (set<string>::const_iterator iter = removed.begin(); iter != removed.end(); ++iter) {
        removeBuf(*iter);
    }
    
    for (set<string>::const_iterator iter = added.begin(); iter != added.end(); ++iter) {
        addBuf(*iter);
    }
}

void UnitNode::addBuf(const string& name)
{
    if (_sprite && _bufs.find(name) == _bufs.end()) {
        DataManager* dm = DataManager::getInstance();
        const SpellConfigData* data = dm->getSpellConfigData(name);
        if (data) {
            const vector<string>& files = data->getResourceNames();
            if (files.size() > 0) {
                const string& file = files.at(0);
                Node* buf = addEffect(file, true);
                if (buf) {
                    const Point& basePos = _sprite->getPosition();
                    const EffectConfigData* ecd = dm->getEffectConfigData(file);
                    if (ecd) {
                        // set zOrder
                        buf->setLocalZOrder(-1);
                        
                        // set position
                        const Point& pos = _configData->getFootEffectPosition();
                        buf->setPosition(basePos + pos);
                        // set scale
                        buf->setScale(_configData->getFootEffectScaleX(), _configData->getFootEffectScaleY());
                    } else {
                        buf->setPosition(basePos/* + _configData->getBodyEffectPosition()*/);
                        buf->setScale(_configData->getBodyEffectScaleX(), _configData->getBodyEffectScaleY());
                    }
                    _bufs.insert(make_pair(name, buf));
                }
            }
        }
    }
}

void UnitNode::removeBuf(const string& name)
{
    if (_bufs.find(name) != _bufs.end()) {
        Node* buf = _bufs.at(name);
        if (buf) {
            buf->removeFromParent();
        }
        _bufs.erase(name);
    }
}

void UnitNode::removeAllBufs()
{
    for (map<string, Node*>::iterator iter = _bufs.begin(); iter != _bufs.end(); ++iter) {
        Node* buf = iter->second;
        if (buf) {
            buf->removeFromParent();
        }
    }
    
    _bufs.clear();
}

void UnitNode::updateFeatures()
{
    const list<Unit::EventLog>& eventLogs = _unit->getEventLogs();
    for (list<Unit::EventLog>::const_iterator iter = eventLogs.begin(); iter != eventLogs.end(); ++iter) {
        const Unit::EventLog& log = *iter;
        Unit::EventLogType type = log._type;
        if (type == Unit::kEventLogType_DamageOutput ||
            type == Unit::kEventLogType_DamageInupt) {
            DamageNature dn = log._damageNature;
            if (dn == kDamageNature_Hurt) {
                // TODO:
            } else if (dn == kDamageNature_Heal) {
                static string file("huifu-2.csb");
                addEffect(file);
            }
        } else if (type == Unit::kEventLogType_FeatureTakeEffect) {
            const FeatureType* ft = log._featureType;
            if (ft) {
                const string& renderKey = ft->getRenderKey();
                const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(renderKey);
                if (data) {
                    const vector<string>& files = data->getResourceNames();
                    if (files.size() > 0) {
                        const string& file = files.at(0);
                        addEffect(file);
                    }
                }
            }
        }
    }
    
    if (_observer) {
        _observer->onUnitNodeUpdatedFeatures(_unit->getUnitId());
    }
}

void UnitNode::addHPBar()
{
    if (!_hpBar && _unit) {
        _hpBar = DisplayBar::create(kHP, _unit->getBelongFaction()->getFactionIndex(), _unit->getUnitBase().getUnitClass());
        const Size& size = _sprite->getContentSize();
        const Point& pos = _sprite->getPosition();
        float offsetY(0);
        if (_configData) {
            offsetY = _configData->getHpBarPosY();
        }
        const Point position(pos + Point(0, size.height / 2 + 10.0f + offsetY));
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
        static const string file("GameImages/effects/backcircle.png");
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
        Node* effect = addEffect(file, false);
        effect->setPosition(_sprite->getPosition());
        return effect;
    }
    
    return nullptr;
}

Node* UnitNode::addEffect(const string& file, bool loop)
{
    if (file.length() > 0) {
        Node *effect = CSLoader::createNode(file);
        if (_needToFlip != _isLastFlipped) {
            node_flipX(effect);
        }
        addChild(effect);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, loop);
        if (!loop) {
            action->setLastFrameCallFunc([effect]() {
                effect->removeFromParent();
            });
        }
        
        return effect;
    }
    
    return nullptr;
}
