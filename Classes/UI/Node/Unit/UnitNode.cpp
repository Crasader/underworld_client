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
#include "SoundManager.h"
#include "CocosUtils.h"
#include "CocosGlobal.h"

using namespace std;
using namespace UnderWorld::Core;

#pragma mark ======================= static =======================
static const int bottomZOrder(-1);
static const int topZOrder(1);
static const float hpPercentageThreshold(50.0f);
static const string rollHintScheduleKeyPrefix("ROLL_HINT_SCHEDULE_KEY_");
static const string rollHintScheduleKeySplitor("_");

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
    const float x = node->getScaleX();
    node->setScale(scale);
    if ((x > 0) ^ (scale > 0)) {
        node_flipX(node);
    }
}

static void node_setScale(Node* node, float scaleX, float scaleY)
{
    const float x = node->getScaleX();
    node->setScale(scaleX, scaleY);
    if ((x > 0) ^ (scaleX > 0)) {
        node_flipX(node);
    }
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
,_rollHintCounter(0)
,_baseSpeed(1.0f)
,_baseScale(1.0f)
,_extraCasterScale(1.0f)
{
    _unitName = unit->getUnitBase().getRenderKey();
    
    UnitClass uc = unit->getUnitBase().getUnitClass();
    _isBuilding = (kUnitClass_Core == uc || kUnitClass_Building == uc);
    //Temp code
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
//            _idLabel = CocosUtils::createLabel(StringUtils::format("%d", _unit->getUnitId()), 40.f);
//            _idLabel->setPosition(0.f, 50.f);
//            this->addChild(_idLabel, 9999);
            
        }
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
            const string& file = prefix + StringUtils::format("-skill-%d.csb", getResourceId(direction));
            if (FileUtils::getInstance()->isFileExist(file)) {
                csbFile = file;
            } else {
                getAttackCsbFiles(output, direction, isHealthy);
            }
        }
            break;
        case kSkillClass_Die:
        {
            if (_isBuilding) {
                csbFile = _configData->getBDestroyed();
            }
            
            if (0 == csbFile.length() || !FileUtils::getInstance()->isFileExist(csbFile)) {
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
    }
    
    if (0 == csbFile.length() || !FileUtils::getInstance()->isFileExist(csbFile)) {
        const string& prefix = _configData->getPrefix();
        csbFile = prefix + StringUtils::format("-standby-%d.csb", getResourceId(direction));
    }
    
    return csbFile;
}

void UnitNode::getAttackCsbFiles(vector<string>& output, Unit::Direction direction, bool isHealthy)
{
    output.clear();
    
    const string& prefix = _configData->getPrefix();
    {
        const string& attack = prefix + StringUtils::format("-attack-%d.csb", getResourceId(direction));
        output.push_back(attack);
    }
    // TODO: handle the unit
    if (_unitName != "巨龙哨兵")
    {
        const string& backSing = prefix + StringUtils::format("-attack-%d-1.csb", getResourceId(direction));
        output.push_back(backSing);
    }
    
    bool isExist(true);
    for (auto iter = begin(output); iter != end(output); ++iter) {
        const string& csbFile = *iter;
        if (0 == csbFile.length() || !FileUtils::getInstance()->isFileExist(csbFile)) {
            isExist = false;
            break;
        }
    }
    
    if (!isExist) {
        output.clear();
        output.push_back(getStandbyCsbFile(direction, isHealthy));
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
        Texture2D::PixelFormat defaultPixelFormat = Texture2D::getDefaultAlphaPixelFormat();
        // TODO: if we change the "PixelFormat", it may cause some display bugs(eg: the shadow of the units)
        if (false) {
            if (defaultPixelFormat != LOW_PIXELFORMAT) {
                Texture2D::setDefaultAlphaPixelFormat(LOW_PIXELFORMAT);
            }
        }
        
        _actionNode = CSLoader::createNode(file);
        _sprite = dynamic_cast<Sprite*>(*(_actionNode->getChildren().begin()));
        addChild(_actionNode);
        
        if (!_isBuilding) {
            if (defaultPixelFormat != Texture2D::getDefaultAlphaPixelFormat()) {
                Texture2D::setDefaultAlphaPixelFormat(defaultPixelFormat);
            }
        }
        
        // flip if needed
        if (flip) {
            node_flipX(_actionNode);
        }
        
        if (play) {
            // 1. add scheduler
            if (!_speedScheduler) {
                _speedScheduler = new (nothrow) Scheduler();
                Director::getInstance()->getScheduler()->scheduleUpdate(_speedScheduler, 0, false);
                if (!_actionManager) {
                    _actionManager = new (nothrow) ActionManager();
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
            
            float scale(1.0f);
            float speed(1.0f);
            DataManager::getInstance()->getAnimationParameters(_unitName, unit_getSkillClass(_unit), _lastDirection, scale, speed);
            
            // set scale
            if (scale != 1.0f && scale > 0) {
                _baseScale = scale;
                scaleActionNode();
            }
            
            // the attack animation should be fit for preperforming time
            if (speed == 1.0f && playTime > 0.0f) {
                const float animationDuration((float)_currentAction->getDuration() / 60.0f);
                speed = animationDuration / playTime;
            }
            
            if (speed != 1.0f && speed > 0) {
                _baseSpeed = speed;
            }
            
            scheduleSpeed();
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
            
            addSwordEffect();
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
        const bool isNewCreated(nullptr == _actionNode);
        
        // 1. remove the old action node
        removeActionNode();
        
        const SkillClass skillClass(unit_getSkillClass(_unit));
        const bool isDead = (kSkillClass_Die == skillClass);
        
        // 2. remove effects
        if (isDead) {
            onDead();
        }
        
        // 3. add the new action node
        const UnitClass uc = _unit->getUnitBase().getUnitClass();
        {
            // attack
            if (kSkillClass_Attack == skillClass && !_isStandby) {
                _isPlayingAttackAnimation = true;
                const float playTime = skill->getTotalPrePerformFrames() / (float)GameConstants::FRAME_PER_SEC;
                addNextAttackActionNode(playTime, frameIndex);
            } else {
                const string& file = _animationFiles.front();
                // die
                const bool isBuilding(kUnitClass_Building == uc);
                if (isDead) {
                    // the unit might has been destroyed when animation finished,
                    // so save the unitId before playing the animation
                    const int unitId = _unit->getUnitId();
                    addActionNode(file, true, false, 0.0f, frameIndex, flip, [=]() {
                        if (_observer) {
                            _observer->onUnitNodePlayDeadAnimationFinished(unitId);
                        }
                    });
                    
                    if (isBuilding) {
                        if (_observer) {
                            _observer->onUnitNodeShakeScreen(this);
                        }
                    }
                } else {
                    // run / standby / cast
                    addActionNode(file, !isBuilding, true, 0.0f, frameIndex, flip, nullptr);
                    
                    if (kSkillClass_Cast == skillClass) {
                        const Spell* spell = dynamic_cast<const Spell*>(_unit->getCurrentSkill());
                        if (spell) {
                            const string& spellName = spell->getSpellType()->getAlias();
                            const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(spellName);
                            if (data) {
                                const vector<string>& resources = data->getCasterResourceNames();
                                const size_t cnt = resources.size();
                                if (cnt > 0) {
                                    addEffect(resources.at(0));
                                    
                                    if (cnt > 1) {
                                        addEffect(resources.at(1), data->getReceiverSpellDirection(), SpellConfigData::kFoot, false, false, nullptr);
                                        
                                        if (cnt > 2) {
                                            Node* node = addEffect(resources.at(2));
                                            node->setPosition(node->getPosition() + Point(0, 87));
                                        }
                                    }
                                }
                                
                                const bool shakeScreen = data->isCasterShakeScreen();
                                if (shakeScreen && _observer) {
                                    _observer->onUnitNodeShakeScreen(this);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // 4. add HP bar
        if (!isDead) {
            addHPBar();
            updateHPBar();
        }
        
        if (isNewCreated && !_isBuilding) {
            const bool isThisFaction(_unit->getWorld()->getThisFactionIndex() == _unit->getBelongFaction()->getFactionIndex());
            if (isThisFaction) {
                addEffect("chuchang-huanrao.csb");
            }
            
#if false
            if (kUnitClass_Hero == uc) {
                string file;
                if (isThisFaction) {
                    file = "hero-quan.csb";
                } else {
                    file = "hero-quan-1.csb";
                }
                addEffect(file, SpellConfigData::kNone, SpellConfigData::kFoot, false, true, nullptr);
            }
#endif
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
        _baseScale = 1.0f;
        _baseSpeed = 1.0f;
    }
}

void UnitNode::scheduleSpeed()
{
    if (_speedScheduler) {
        float extraSpeed(1.0f);
        const SkillClass skillClass(unit_getSkillClass(_unit));
        if (_extraBufSpeeds.find(skillClass) != _extraBufSpeeds.end()) {
            extraSpeed = _extraBufSpeeds.at(skillClass);
        }
        
        const float speed = _baseSpeed * extraSpeed;
        
        if (speed != 1.0f) {
            _speedScheduler->setTimeScale(speed);
        } else if (_speedScheduler->getTimeScale() != 1.0f) {
            _speedScheduler->setTimeScale(1.0f);
        }
    }
}

void UnitNode::scaleActionNode()
{
    if (_actionNode) {
        float extraScale(1.0f);
        const SkillClass skillClass(unit_getSkillClass(_unit));
        if (_extraBufScales.find(skillClass) != _extraBufScales.end()) {
            extraScale = _extraBufScales.at(skillClass);
        }
        
        const float scale = _baseScale * extraScale;
        node_setScale(_actionNode, scale);
    }
}

#pragma mark - effects
void UnitNode::updateBufs()
{
    auto last = _bufNames;
    
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
    
    for (auto iter = removed.begin(); iter != removed.end(); ++iter) {
        removeBuf(*iter);
    }
    
    ssize_t cnt = added.size();
    if (cnt > 0) {
        // re-calculate the buf scale
        for (auto iter = added.begin(); iter != added.end(); ++iter) {
            const string& name = *iter;
            const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(name);
            if (data) {
                {
                    const auto& rates = data->getReceiverSpeedRates();
                    for (auto iter = rates.begin(); iter != rates.end(); ++iter) {
                        const SkillClass sc = iter->first;
                        const float rate = iter->second;
                        if (_extraBufSpeeds.find(sc) != _extraBufSpeeds.end()) {
                            const float extraRate = _extraBufSpeeds.at(sc);
                            _extraBufSpeeds.at(sc) = rate + extraRate;
                        } else {
                            _extraBufSpeeds.insert(make_pair(sc, rate));
                        }
                    }
                }
                
                {
                    const auto& rates = data->getReceiverVolumeRates();
                    for (auto iter = rates.begin(); iter != rates.end(); ++iter) {
                        const SkillClass sc = iter->first;
                        const float rate = iter->second;
                        if (_extraBufScales.find(sc) != _extraBufScales.end()) {
                            const float extraRate = _extraBufScales.at(sc);
                            _extraBufScales.at(sc) = rate + extraRate;
                        } else {
                            _extraBufScales.insert(make_pair(sc, rate));
                        }
                    }
                }
                
                addBuf(*iter);
            }
        }
        
        // calculate average
        {
            for (auto iter = _extraBufSpeeds.begin(); iter != _extraBufSpeeds.end(); ++iter) {
                const float sum = iter->second;
                iter->second = sum / cnt;
            }
        }
        
        {
            for (auto iter = _extraBufScales.begin(); iter != _extraBufScales.end(); ++iter) {
                const float sum = iter->second;
                iter->second = sum / cnt;
            }
        }
    }
    
    scheduleSpeed();
    scaleActionNode();
}

void UnitNode::addBuf(const string& name)
{
    if (_sprite && _bufs.find(name) == _bufs.end()) {
        DataManager* dm = DataManager::getInstance();
        const SpellConfigData* data = dm->getSpellConfigData(name);
        if (data) {
            const vector<string>& files = data->getReceiverResourceNames();
            if (files.size() > 0) {
                const string& file = files.at(0);
                Node* buf = addEffect(file, data->getReceiverSpellDirection(), data->getReceiverSpellPosition(), true, true, nullptr);
                if (buf) {
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
    for (auto iter = _bufs.begin(); iter != _bufs.end(); ++iter) {
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
    for (auto iter = eventLogs.begin(); iter != eventLogs.end(); ++iter) {
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
                    const vector<string>& files = data->getReceiverResourceNames();
                    if (files.size() > 0) {
                        const string& file = files.at(0);
                        addEffect(file, data->getReceiverSpellDirection(), SpellConfigData::kBody, true, false, nullptr);
                    }
                }
            }
        } else if (type == Unit::kEventLogType_ResourceOutput) {
            if (log._factionIndex == _unit->getWorld()->getThisFactionIndex()) {
                float delay = 0.3f;
                int index = 0;
                for (auto iter = log._resources.begin();
                     iter != log._resources.end();
                     ++iter) {
                    string resource = iter->first;
                    float amount = iter->second;
                    string scheduleKey = rollHintScheduleKeyPrefix
                        + to_string(_unit->getUnitId())
                        + rollHintScheduleKeySplitor
                        + to_string(++_rollHintCounter);
                    rollHintResource(resource, amount, delay * index);
                    ++index;
                }
            }
        }
    }
    
    _unit->clearEventLogs();
}

void UnitNode::addHPBar()
{
    if (!_hpBar && _unit && _sprite) {
        _hpBar = DisplayBar::create(DisplayBarType::HP, _unit->getBelongFaction()->getFactionIndex(), _unit->getUnitBase().getUnitClass());
        _hpBar->setPosition(getHPBarPosition());
        addChild(_hpBar, topZOrder);
        if (_configData) {
            node_setScale(_hpBar, _configData->getHpBarScaleX(), 1.0f);
        }
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

Point UnitNode::getHPBarPosition() const
{
    if (_hpBar && (Point::ZERO != _hpBar->getPosition())) {
        return _hpBar->getPosition();
    } else if (_sprite) {
        const Size& size = _sprite->getContentSize();
        const Point& pos = _sprite->getPosition();
        float offsetX(0);
        float offsetY(0);
        if (_configData) {
            offsetX = _configData->getHpBarPosX();
            offsetY = _configData->getHpBarPosY();
        }
        
        Point position(pos + Point(offsetX, size.height / 2 + offsetY));
        position = convertToNodeSpace(_actionNode->convertToWorldSpace(position));
        return position;
    }
    
    return Point::ZERO;
}

void UnitNode::addShadow()
{
    if (!_shadow) {
        static const string file("GameImages/effects/backcircle.png");
        const Point& pos = _sprite->getPosition();
        _shadow = Sprite::create(file);
        _shadow->setPosition(pos + _configData->getFootEffectPosition());
        addChild(_shadow, bottomZOrder);
    }
}

void UnitNode::removeShadow()
{
    if (_shadow) {
        _shadow->removeFromParent();
        _shadow = nullptr;
    }
}

void UnitNode::addSwordEffect()
{
    const string& file = StringUtils::format(_configData->getSwordEffect().c_str(), _lastDirection);
    if (file.length() > 0) {
        addEffect(file);
    }
}

Node* UnitNode::addEffect(const string& file)
{
    if (_sprite) {
        Node* effect = addEffect(file, SpellConfigData::kNone, SpellConfigData::kBody, false, false, nullptr);
        return effect;
    }
    
    return nullptr;
}

Node* UnitNode::addEffect(const string& file,
                          const SpellConfigData::SpellDirection& direction,
                          const SpellConfigData::SpellPosition& position,
                          bool scale,
                          bool loop,
                          const function<void()>& callback)
{
    if (!_sprite || file.empty()) {
        return nullptr;
    }
    
    const size_t found = file.find_last_of(".");
    if (found != string::npos) {
        const string& suffix = file.substr(found + 1);
        if ("csb" == suffix) {
            Node *effect = CSLoader::createNode(file);
            bool flip(false);
            if (SpellConfigData::kNone != direction) {
                const bool isFaceRight = _needToFlip ^ _configData->isFaceRight();
                flip = (isFaceRight ^ (SpellConfigData::kRight == direction));
            }
            
            if (flip ^ (_needToFlip != _isLastFlipped)) {
                node_flipX(effect);
            }
            
            addChild(effect, topZOrder);
            cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
            effect->runAction(action);
            action->gotoFrameAndPlay(0, loop);
            if (!loop) {
                action->setLastFrameCallFunc([effect, callback]() {
                    effect->removeFromParent();
                    if (callback) {
                        callback();
                    }
                });
            } else {
                assert(!callback);
            }
            
            // set position
            if (SpellConfigData::kFoot == position) {
                effect->setPosition(_configData->getFootEffectPosition());
                if (scale) {
                    node_setScale(effect, _configData->getFootEffectScaleX() * _baseScale, _configData->getFootEffectScaleY() * _baseScale);
                } else {
                    node_setScale(effect, _configData->getFootEffectScaleX(), _configData->getFootEffectScaleY());
                }
                effect->setLocalZOrder(bottomZOrder);
            } else if (SpellConfigData::kBody == position) {
                effect->setPosition(_sprite->getPosition());
                if (scale) {
                    node_setScale(effect, _baseScale);
                }
            } else if (SpellConfigData::kHead == position) {
                effect->setPosition(getHPBarPosition());
                if (scale) {
                    node_setScale(effect, _baseScale);
                }
            }
            
            return effect;
        } else if ("plist" == suffix) {
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            if (!loop) {
                effect->setAutoRemoveOnFinish(true);
            }
            addChild(effect, topZOrder);
            return effect;
        }
    }
    
    return nullptr;
}

void UnitNode::rollHintResource(const std::string &resource, float amount, float delay) {
    Node* hintNode = Node::create();
    Node* iconNode = nullptr;
    if (resource == RES_NAME_GOLD) {
        iconNode = Sprite::create(StringUtils::format("GameImages/resources/icon_%dB.png", ::ResourceType::Gold));
    } else if (resource == RES_NAME_WOOD) {
        iconNode = Sprite::create(StringUtils::format("GameImages/resources/icon_%dB.png", ::ResourceType::Wood));
    }
    assert(iconNode);
    iconNode->setAnchorPoint(Vec2(1.f, 0.5f));
    iconNode->setScale(0.5f);
    
    auto amountNode = CocosUtils::create10x25Number("+" + to_string(amount));
    amountNode->setAnchorPoint(Vec2(0.f, 0.5f));
    
    hintNode->addChild(iconNode);
    hintNode->addChild(amountNode);
    rollHintNode(hintNode, delay);
}

void UnitNode::rollHintNode(cocos2d::Node *hintNode, float delay) {
    if (!hintNode) return;
    hintNode->setCascadeOpacityEnabled(true);
    hintNode->setPosition(this->getPosition());
    hintNode->setVisible(false);
    this->getParent()->addChild(hintNode);
    float duration = 2.0f;
    Sequence* fadeTo = Sequence::create(DelayTime::create(1.5f), FadeTo::create(duration - 1.5f, 0), NULL);
    MoveBy* moveBy = MoveBy::create(duration, Vec2(0.f, 100.f));
    hintNode->runAction(Sequence::create(
        DelayTime::create(delay),
        CallFunc::create([hintNode](){ hintNode->setVisible(true); }),
        Spawn::create(fadeTo, moveBy, NULL),
        RemoveSelf::create(), NULL));
}
