//
//  UnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UnitNode.h"
#include "JSonUtils.h"
#include "DataManager.h"
#include "AnimationParameter.h"
#include "URConfigData.h"
#include "SoundManager.h"
#include "Spell.h"
#include "Game.h"
#include "CoreUtils.h"
#include "CocosUtils.h"
#include "DisplayBar.h"

using namespace std;

static const int zOrder_bottom(-1);
static const int zOrder_top(1);
static const float hpThreshold(50.0f);

#pragma mark - AnimationType
class UnitNode::AnimationType {
public:
    AnimationType() {}
    void assign(const AnimationType& instance) {
        _file.assign(instance._file);
        _startIdx = instance._startIdx;
        _endIdx = instance._endIdx;
    }
    void assign(const string& file, int startIdx = 0, int endIdx = -1) {
        _file.assign(file); _startIdx = startIdx; _endIdx = endIdx;
    }
    const string& getFile() const { return _file; }
    int getStartIdx() const { return _startIdx; }
    int getEndIdx() const { return _endIdx; }
    
private:
    string _file;
    int _startIdx;
    int _endIdx;
};

#pragma mark - AnimationNode
struct UnitNode::AnimationNode {
    string nodeFile;
    Node* node;
    string shadowFile;
    Node* shadow;
};

#pragma mark - UnitNode
UnitNode* UnitNode::create(const Unit* unit, bool rightSide)
{
    UnitNode *ret = new (nothrow) UnitNode(unit, rightSide);
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitNode::UnitNode(const Unit* unit, bool rightSide)
:_observer(nullptr)
,_unit(unit)
,_body(nullptr)
,_equipment(nullptr)
,_sprite(nullptr)
,_animation(nullptr)
,_speedScheduler(nullptr)
,_actionManager(nullptr)
,_hpBar(nullptr)
,_idLabel(nullptr)
,_skill(nullptr)
,_direction(static_cast<Unit::Direction>(-1))
,_isHealthy(true)
,_isFlipped(false)
,_isStandby(false)
,_isAttacking(false)
,_attackAnimationsCounter(0)
,_baseParams(nullptr)
,_rollHintCounter(0)
{
    _unitName = unit->getUnitBase().getRenderKey();
    _configData = DataManager::getInstance()->getURConfigData(_unitName);
    if (_configData->getAnimationType() == UnitAnimationType::PVR) {
        _needToFlip = !rightSide;
    } else {
        _needToFlip = (rightSide == _configData->isFaceRight());
    }
    UnitClass uc(thisUnitClass());
    _isBuilding = (kUnitClass_Core == uc || kUnitClass_Building == uc);
    _body = new (nothrow) AnimationNode();
    _equipment = new (nothrow) AnimationNode();
    _baseParams = new (nothrow) AnimationParameter(nullptr);
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
    
    CC_SAFE_DELETE(_body);
    CC_SAFE_DELETE(_equipment);
    CC_SAFE_DELETE(_baseParams);
    
    for (auto iter = begin(_extraParams); iter != end(_extraParams); ++iter) {
        CC_SAFE_DELETE(iter->second);
    }
    
    removeAllChildren();
}

#pragma mark - node
bool UnitNode::init()
{
    if (Node::init()) {
        if (_configData->getAnimationType() == UnitAnimationType::PVR) {
            scheduleUpdate();
        }
        return true;
    }
    
    return false;
}

void UnitNode::update(float dt)
{
    if (_animation) {
        if (kSkillClass_Attack == thisSkillClass()) {
            CCLOG("%d", getCurrentFrameIndex());
        }
    }
}

void UnitNode::setOpacity(GLubyte opacity)
{
    Node::setOpacity(opacity);
    
    if (_sprite) {
        _sprite->setOpacity(opacity);
    }
}

#pragma mark - public
void UnitNode::registerObserver(UnitNodeObserver *observer)
{
    _observer = observer;
}

const Unit* UnitNode::getUnit() const
{
    return _unit;
}

void UnitNode::update(const Game* game)
{
    if (_unit) {
        auto skill = _unit->getCurrentSkill();
        if (skill) {
            // direction and flip
            auto direction = _unit->getDirection();
            bool flip = needToFlip(direction);
            bool update(false);
            int frameIndex(0);
            const float percentage(getHpPercentage());
            const bool isHealthy = (percentage > hpThreshold);
            
            if (_skill) {
                auto sc(thisSkillClass());
                auto lastSc(getSkillClass(_skill));
                auto uc(thisUnitClass());
                
                if (kUnitClass_Core == uc) {
                    if (sc != lastSc && kSkillClass_Die == sc) {
                        // destroyed
                        update = true;
                    } else if (isHealthy != _isHealthy) {
                        // hurt
                        update = true;
                    }
                } else {
                    if (sc != lastSc) {
                        // check if the unit is standby
                        if (needToChangeStandbyStatus()) {
                            _isStandby = !_isStandby;
                        } else if (_isStandby) {
                            _isStandby = false;
                        }
                        
                        update = true;
                    } else if (_direction != direction || _isFlipped != flip) {
                        // check if the unit is standby
                        if (needToChangeStandbyStatus()) {
                            _isStandby = !_isStandby;
                        }
                        
                        update = true;
                        frameIndex = getCurrentFrameIndex();
                    } else {
                        // check if the unit is standby
                        if (needToChangeStandbyStatus()) {
                            _isStandby = !_isStandby;
                            update = true;
                        }
                    }
                }
            } else {
                update = true;
            }
            
            if (update) {
                // reset
                _skill = skill;
                _direction = direction;
                _isHealthy = isHealthy;
                _isFlipped = flip;
                resetAttackParams();
                
                // play
                getAnimationFiles(_animationFiles, thisSkillClass(), direction, isHealthy);
                updateAnimation(skill, frameIndex, flip, game);
            }
        }
        
        updateHPBar();
        updateBufs();
        updateFeatures(game);
        
        /*
         if (!_idLabel) {
         _idLabel = CocosUtils::createLabel(StringUtils::format("%d", _unit->getUnitId()), 40.f);
         _idLabel->setPosition(0.f, 50.f);
         this->addChild(_idLabel, 9999);
         
         }*/
    }
}

#pragma mark callbacks
void UnitNode::onHurt(const string& trigger)
{
    const URConfigData* data = DataManager::getInstance()->getURConfigData(trigger);
    if (data) {
        playSound(data->getHurtSound());
        //addEffect(data->getHurtEffect());
    }
}

void UnitNode::onDead()
{
    notifyRemoveShadow();
    removeAllBufs();
    removeHPBar();
    playSound(_configData->getDieSound());
    
    // TODO: remove irregular code
    setLocalZOrder(-1000);
}

void UnitNode::onWin()
{
    
}

void UnitNode::onLose()
{
    
}

#pragma mark - universal
void UnitNode::flipX(Node* node) const
{
    CS_RETURN_IF(!node);
    const float scaleX = node->getScaleX();
    node->setScaleX(-1 * scaleX);
}

void UnitNode::scale(Node* node, float scale) const
{
    CS_RETURN_IF(!node);
    const float x = node->getScaleX();
    node->setScale(scale);
    if ((x > 0) ^ (scale > 0)) {
        flipX(node);
    }
}

void UnitNode::scale(Node* node, float scaleX, float scaleY) const
{
    CS_RETURN_IF(!node);
    const float x = node->getScaleX();
    node->setScale(scaleX, scaleY);
    if ((x > 0) ^ (scaleX > 0)) {
        flipX(node);
    }
}

void UnitNode::playSound(const string& file) const
{
    if (file.length() > 0) {
        SoundManager::getInstance()->playSound(file);
    }
}

#pragma mark - getters
string UnitNode::getShadowFile(const string& file, bool flip) const
{
    if (!file.empty() && _configData->getAnimationType() == UnitAnimationType::PVR) {
        string ret;
        static const string separator("body");
        auto pos = file.find(separator);
        if (string::npos != pos) {
            ret.assign(file);
            ret.replace(pos, separator.size(), StringUtils::format("shadows/%d", flip ? 0 : 1));
            if (_isBuilding) {
                ret = ret.substr(0, ret.rfind('/'));
            }
        }
        
        return ret;
    }
    
    return "";
}

string UnitNode::getEquipmentFile(const string& file, bool flip) const
{
    if (!file.empty() && _configData->getAnimationType() == UnitAnimationType::PVR) {
        string ret;
        auto pos = file.find_first_of("/");
        if (string::npos != pos) {
            ret = file.substr(0, pos) + "-equipment" + file.substr(pos, file.size() - 1);
        }
        
        return ret;
    }
    
    return "";
}

int UnitNode::getResourceId(Unit::Direction direction) const
{
    if (_configData->getAnimationType() == UnitAnimationType::PVR) {
        switch (direction) {
            case Unit::kDirection_Up:
                return 0;
            case Unit::kDirection_leftUp2:
            case Unit::kDirection_rightUp2:
                return 1;
            case Unit::kDirection_leftUp1:
            case Unit::kDirection_rightUp1:
                return 2;
            case Unit::kDirection_right:
            case Unit::kDirection_left:
                return 3;
            case Unit::kDirection_leftDown1:
            case Unit::kDirection_rightDown1:
                return 4;
            case Unit::kDirection_leftDown2:
            case Unit::kDirection_rightDown2:
                return 5;
            case Unit::kDirection_Down:
                return 6;
                
            default:
                assert(false);
                return 0;
        }
    } else {
        switch (direction) {
            case Unit::kDirection_Up:
            case Unit::kDirection_rightUp1:
            case Unit::kDirection_leftUp1:
            case Unit::kDirection_rightUp2:
            case Unit::kDirection_leftUp2:
                return 2;
            case Unit::kDirection_right:
            case Unit::kDirection_left:
                return 3;
            case Unit::kDirection_rightDown1:
            case Unit::kDirection_leftDown1:
            case Unit::kDirection_rightDown2:
            case Unit::kDirection_leftDown2:
            case Unit::kDirection_Down:
                return 4;
                
            default:
                assert(false);
                return 0;
        }
    }
}

int UnitNode::thisFactionIndex() const
{
    return _unit->getBelongFaction()->getFactionIndex();
}

UnitClass UnitNode::thisUnitClass() const
{
    return _unit->getUnitBase().getUnitClass();
}

SkillClass UnitNode::thisSkillClass() const
{
    return getSkillClass(_unit->getCurrentSkill());
}

SkillClass UnitNode::getSkillClass(const Skill* skill) const
{
    if (skill) {
        return skill->getSkillType()->getSkillClass();
    }
    
    assert(false);
    return kSkillClass_Stop;
}

float UnitNode::getHpPercentage() const
{
    const int maxHp = _unit->getUnitBase().getMaxHp();
    const int hp = _unit->getHp();
    return 100 * (float)hp / (float)maxHp;
}

bool UnitNode::needToChangeStandbyStatus() const
{
    if (kSkillClass_Attack == thisSkillClass()) {
        bool isStandby(false);
        
        // check if the unit is standby
        auto state(_unit->getCurrentSkill()->getSkillState());
        if (!_isAttacking && Skill::SkillState::kSkillState_idle == state) {
            isStandby = true;
        }
        
        return (_isStandby != isStandby) ? true : false;
    }
    
    return false;
}

bool UnitNode::needToFlip(Unit::Direction direction) const
{
    bool flip(false);
    if (_unit) {
        const bool isFaceRight(_configData->getAnimationType() == UnitAnimationType::PVR ? false : _configData->isFaceRight());
        
        switch (direction) {
            case Unit::kDirection_right:
            case Unit::kDirection_rightUp1:
            case Unit::kDirection_rightUp2:
            case Unit::kDirection_rightDown1:
            case Unit::kDirection_rightDown2:
            {
                flip = !isFaceRight;
            }
                break;
            case Unit::kDirection_left:
            case Unit::kDirection_leftUp1:
            case Unit::kDirection_leftUp2:
            case Unit::kDirection_leftDown1:
            case Unit::kDirection_leftDown2:
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

void UnitNode::getAnimationFiles(vector<AnimationType>& output,
                                 SkillClass sc,
                                 Unit::Direction direction,
                                 bool isHealthy) const
{
    output.clear();
    
    AnimationType data;
    
    if (_configData->getAnimationType() == UnitAnimationType::PVR) {
        if (_isBuilding) {
            const string suffix(0 == thisFactionIndex() ? "Blue" : "Red");
            data.assign(_configData->getNormalPrefix() + "/body/" + suffix);
        } else {
            string prefix("");
            switch (sc) {
                case kSkillClass_Stop:
                case kSkillClass_Produce:
                {
                    prefix = "stand";
                }
                    break;
                case kSkillClass_Move:
                {
                    prefix = "run";
                }
                    break;
                case kSkillClass_Attack:
                {
                    if (_isStandby) {
                        prefix = "stand";
                    } else {
                        prefix = "attack";
                        getSegmentalFiles(output, prefix, direction, isHealthy);
                    }
                }
                    break;
                case kSkillClass_Cast:
                {
                    prefix = "skill";
                }
                    break;
                case kSkillClass_Die:
                {
                    prefix = "stand";
                }
                    break;
                    
                default:
                    break;
            }
            
            if (0 == output.size()) {
                data.assign(_configData->getNormalPrefix() + "/" + prefix + StringUtils::format("/body/%d", getResourceId(direction)));
            }
        }
    } else {
        const auto& prefix = _configData->getNormalPrefix();
        switch (sc) {
            case kSkillClass_Stop:
            case kSkillClass_Produce:
            {
                getStandbyFiles(data, direction, isHealthy);
            }
                break;
            case kSkillClass_Move:
            {
                if (_isBuilding) {
                    assert(false);
                } else {
                    data.assign(prefix + StringUtils::format("-run-%d.csb", getResourceId(direction)));
                }
            }
                break;
            case kSkillClass_Attack:
            {
                if (_isStandby) {
                    getStandbyFiles(data, direction, isHealthy);
                } else {
                    getSegmentalFiles(output, "attack", direction, isHealthy);
                }
            }
                break;
            case kSkillClass_Cast:
            {
                const auto& file = prefix + StringUtils::format("-skill-%d.csb", getResourceId(direction));
                if (FileUtils::getInstance()->isFileExist(file)) {
                    data.assign(file);
                } else {
                    getSegmentalFiles(output, "attack", direction, isHealthy);
                }
            }
                break;
            case kSkillClass_Die:
            {
                if (_isBuilding) {
                    data.assign(_configData->getNormalPrefix());
                }
                
                const auto& file(data.getFile());
                if (0 == file.length() || !FileUtils::getInstance()->isFileExist(file)) {
                    data.assign(prefix + StringUtils::format("-dead-%d.csb", getResourceId(direction)));
                }
            }
                break;
            default:
                assert(false);
                break;
        }
    }
    
    if (0 == output.size() && 0 < data.getFile().size()) {
        output.push_back(data);
    }
}

void UnitNode::getStandbyFiles(AnimationType& output,
                               Unit::Direction direction,
                               bool isHealthy) const
{
    output.assign("");
    
    if (_configData->getAnimationType() == UnitAnimationType::PVR) {
        vector<AnimationType> vs;
        getAnimationFiles(vs, kSkillClass_Stop, direction, isHealthy);
        if (vs.size() > 0) {
            output.assign(vs.at(0));
        }
    } else {
        if (_isBuilding) {
            const auto& normal = StringUtils::format(_configData->getNormalPrefix().c_str(), getResourceId(direction));
            const auto& damaged = StringUtils::format(_configData->getNormalPrefix().c_str(), getResourceId(direction));
            output.assign(isHealthy ? normal : (damaged.length() > 0 ? damaged : normal));
        }
        
        const auto& file(output.getFile());
        if (0 == file.length() || !FileUtils::getInstance()->isFileExist(file)) {
            const auto& prefix = _configData->getNormalPrefix();
            output.assign(prefix + StringUtils::format("-standby-%d.csb", getResourceId(direction)));
        }
    }
}

void UnitNode::getSegmentalFiles(vector<AnimationType>& output,
                                 const string& mark,
                                 Unit::Direction direction,
                                 bool isHealthy) const
{
    output.clear();
    
    if (_configData->getAnimationType() == UnitAnimationType::PVR) {
        string file = _configData->getNormalPrefix() + "/" + mark + "/body/" + StringUtils::format("%d", getResourceId(direction));
        int middle(0);
        AnimationType at;
        if (middle > 0) {
            at.assign(file, 0, middle - 1);
            output.push_back(at);
            
            at.assign(file, middle);
            output.push_back(at);
        } else {
            at.assign(file);
            output.push_back(at);
        }
    } else {
        const auto& prefix = _configData->getNormalPrefix();
        {
            AnimationType at;
            at.assign(prefix + "-" + mark + StringUtils::format("-%d.csb", getResourceId(direction)));
            output.push_back(at);
        }
        // TODO: handle the unit
        if (_unitName != "巨龙哨兵")
        {
            AnimationType at;
            at.assign(prefix + "-" + mark + StringUtils::format("-%d-1.csb", getResourceId(direction)));
            output.push_back(at);
        }
        
        bool isExist(true);
        for (auto iter = begin(output); iter != end(output); ++iter) {
            const auto& csbFile = (*iter).getFile();
            if (0 == csbFile.length() || !FileUtils::getInstance()->isFileExist(csbFile)) {
                isExist = false;
                break;
            }
        }
        
        if (!isExist) {
            output.clear();
            AnimationType at;
            getStandbyFiles(at, direction, isHealthy);
            output.push_back(at);
        }
    }
}

int UnitNode::getCurrentFrameIndex() const
{
    if (_animation) {
        if (_configData->getAnimationType() == UnitAnimationType::PVR) {
            auto animation = dynamic_cast<Animate*>(_animation);
            if (animation) {
                return animation->getCurrentFrameIndex();
            }
        } else {
            auto animation = dynamic_cast<cocostudio::timeline::ActionTimeline*>(_animation);
            if (animation) {
                return animation->getCurrentFrame();
            }
        }
    }
    
    return 0;
}

#pragma mark - animation
void UnitNode::playAnimation(const AnimationType& at,
                             bool play,
                             bool loop,
                             float playTime,
                             int frameIndex,
                             bool flip,
                             const function<void(Node*)>& lastFrameCallFunc)
{
    clear();
    
    // TODO: move the temporary code
    bool isHero(false);
    const auto& file(at.getFile());
    if (string::npos != file.find("hero")) {
        isHero = true;
    }
    
    _body->nodeFile.assign(file);
    auto cnt = _body->nodeFile.size();
    if (cnt > 0) {
        // set PixelFormat if needed
        Texture2D::PixelFormat defaultPixelFormat = Texture2D::getDefaultAlphaPixelFormat();
        
        // if we change the "PixelFormat", it may cause some display bugs(eg: the shadow of the units)
        if (false && defaultPixelFormat != LOW_PIXELFORMAT) {
            Texture2D::setDefaultAlphaPixelFormat(LOW_PIXELFORMAT);
        }
        
        // -- body's node -- //
        auto startIdx(at.getStartIdx());
        auto node = CocosUtils::getAnimationNode(_body->nodeFile, startIdx);
        addChild(node);
        _body->node = node;
        
        if (_configData->getAnimationType() == UnitAnimationType::PVR) {
            _sprite = dynamic_cast<Sprite*>(node);
        } else {
            _sprite = dynamic_cast<Sprite*>(node->getChildren().front());
        }
        
        if (_configData->getAnimationType() == UnitAnimationType::PVR) {
            // -- body's shadow -- //
            createShadow(&_body, flip, startIdx);
            
            if (isHero) {
                // -- equipment's node -- //
                createEquipment(&_equipment, _body->nodeFile, flip, startIdx);
                
                // -- equipment's shadow -- //
//                createShadow(&_equipment, flip, startIdx);
            }
        }
        
        if (false && defaultPixelFormat != Texture2D::getDefaultAlphaPixelFormat()) {
            Texture2D::setDefaultAlphaPixelFormat(defaultPixelFormat);
        }
        
        // flip if needed
        if (flip) {
            flipX(node);
        }
        
        if (play) {
            // 1. set scheduler
            setScheduler();
            
            // 2. play animation
            frameIndex += startIdx;
            auto duration = CocosUtils::playAnimation(_sprite, _body->nodeFile, DEFAULT_FRAME_DELAY, loop, frameIndex, at.getEndIdx(), lastFrameCallFunc);
            
            if (_body->shadow && !_body->shadowFile.empty()) {
                CocosUtils::playAnimation(_body->shadow, _body->shadowFile, DEFAULT_FRAME_DELAY, loop, frameIndex, at.getEndIdx());
            }
            
            if (_equipment->node && !_equipment->nodeFile.empty()) {
                CocosUtils::playAnimation(_equipment->node, _equipment->nodeFile, DEFAULT_FRAME_DELAY, loop, frameIndex, at.getEndIdx());
            }
            
            if (_equipment->shadow && !_equipment->shadowFile.empty()) {
                CocosUtils::playAnimation(_equipment->shadow, _equipment->shadowFile, DEFAULT_FRAME_DELAY, loop, frameIndex, at.getEndIdx());
            }
            
            // change scale and speed
            auto parameter = DataManager::getInstance()->getAnimationParameter(_unitName, thisSkillClass(), _direction);
            auto scale = parameter ? parameter->scale : 1.0f;
            auto speed = parameter ? parameter->speed : 1.0f;
            
            // the attack animation should be fit for preperforming time
            if (speed == 1.0f && playTime > 0.0f) {
                speed = duration / playTime;
            }
            
            _baseParams->scale = scale;
            _baseParams->speed = speed;
            
            updateAnimationParams();
        }
    }
}

void UnitNode::updateAnimation(const Skill* skill,
                               int frameIndex,
                               bool flip,
                               const Game* game)
{
    auto cnt = _animationFiles.size();
    if (_unit && cnt > 0) {
        // 0. check whether it's new created before remove node
        auto isNewCreated(!_body->node);
        
        // 1. remove the old action node
        clear();
        
        auto skillClass(thisSkillClass());
        auto isDead(kSkillClass_Die == skillClass);
        
        // 2. remove effects
        if (isDead) {
            onDead();
        }
        
        // 3. add the new action node
        auto uc(thisUnitClass());
        {
            // attack
            if (kSkillClass_Attack == skillClass && !_isStandby) {
                _isAttacking = true;
                const float playTime = skill->getTotalPrePerformFrames() / (float)GameConstants::FRAME_PER_SEC;
                playNextAttackAnimation(playTime, frameIndex);
            } else {
                const auto& file = _animationFiles.front();
                const bool isBuilding(kUnitClass_Building == uc);
                if (isDead) {
                    // die
                    
                    // the unit might has been destroyed when animation finished,
                    // so save the unitId before playing the animation
                    const int unitId = _unit->getId();
                    playAnimation(file, true, false, 0.0f, frameIndex, flip, [=](Node* unused) {
                        if (_observer) {
                            _observer->onUnitNodePlayedDeadAnimation(unitId);
                        }
                    });
                    
                    if (isBuilding) {
                        if (_observer) {
                            _observer->onUnitNodeShakeScreen(this);
                        }
                    }
                } else if (kSkillClass_Cast == skillClass) {
                    // cast
                    const float playTime = skill->getTotalPrePerformFrames() / (float)GameConstants::FRAME_PER_SEC;
                    playAnimation(file, !isBuilding, false, playTime, frameIndex, flip, [this](Node* unused) {
                        playStandbyAnimation();
                    });
                    
                    auto spell = dynamic_cast<const Spell*>(_unit->getCurrentSkill());
                    if (spell) {
                        const auto& spellName = spell->getSpellType()->getAlias();
                        auto data = DataManager::getInstance()->getSpellConfigData(spellName);
                        if (data) {
//                            const auto& resources = data->getCasterResourceNames();
//                            auto cnt = resources.size();
//                            if (cnt > 0) {
//                                addEffect(resources.at(0));
//                                
//                                if (cnt > 1) {
//                                    addAnimation(resources.at(1), false, nullptr, false, data->getReceiverSpellDirection(), SpellConfigData::kFoot);
//                                    
//                                    if (cnt > 2) {
//                                        auto node = addEffect(resources.at(2));
//                                        node->setPosition(node->getPosition() + Point(0, 87));
//                                    }
//                                }
//                            }
//                            
//                            auto shakeScreen(data->isCasterShakeScreen());
//                            if (shakeScreen && _observer) {
//                                _observer->onUnitNodeShakeScreen(this);
//                            }
                        }
                    }
                } else {
                    // run / standby
                    playAnimation(file, !isBuilding, true, 0.0f, frameIndex, flip, nullptr);
                }
            }
        }
        
        // 4. add HP bar
        if (!isDead) {
            addHPBar();
            updateHPBar();
        }
        
        if (isNewCreated && !_isBuilding) {
            const bool isThisFaction(game->getThisFactionIndex() == thisFactionIndex());
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

#pragma mark standby
void UnitNode::playStandbyAnimation()
{
    Unit::Direction direction = _unit->getDirection();
    bool flip = needToFlip(direction);
    AnimationType at;
    getStandbyFiles(at, direction, getHpPercentage() > hpThreshold);
    playAnimation(at, true, true, 0.0f, 0, flip, nullptr);
}

#pragma mark attack
void UnitNode::playNextAttackAnimation(float playTime, int frameIndex)
{
    if (_animationFiles.empty()) {
        _isAttacking = false;
        playStandbyAnimation();
    } else {
        const auto& file = _animationFiles.front();
        playAnimation(file, true, false, playTime, frameIndex, _isFlipped, CC_CALLBACK_0(UnitNode::onAttackAnimationFinished, this));
    }
}

void UnitNode::onAttackAnimationFinished()
{
    // it means the "functionalIndex" of the animation sequence has a callback,
    // the others are only the animations without any function.
    static const int functionalIndex(0);
    
    // 1. execute callback
    if (_attackAnimationsCounter == functionalIndex) {
        // play sound
        playSound(_configData->getAttackSound());
        
//        // if it is footman
//        if (_configData->isShortRange()) {
//            if (_observer) {
//                _observer->onUnitNodeHurtTheTarget(this);
//            }
//            
//            addSwordEffect();
//        }
    }
    
    // 2. play the next animation
    _animationFiles.erase(_animationFiles.begin());
    ++ _attackAnimationsCounter;
    playNextAttackAnimation(0.0f, 0);
    // because only the "functionalIndex" of the animation sequence has some function,
    // in order to avoid getting the incorrect "frameIndex", set "_animation" to null.
    if (_attackAnimationsCounter != functionalIndex) {
        _animation = nullptr;
    }
}

void UnitNode::resetAttackParams()
{
    _isAttacking = false;
    _attackAnimationsCounter = 0;
    _animationFiles.clear();
}

#pragma mark - operations
void UnitNode::setScheduler()
{
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
        if (_body->node) {
            _body->node->setActionManager(_actionManager);
        }
        
        if (_body->shadow) {
            _body->shadow->setActionManager(_actionManager);
        }
        
        if (_equipment->node) {
            _equipment->node->setActionManager(_actionManager);
        }
        
        if (_equipment->shadow) {
            _equipment->shadow->setActionManager(_actionManager);
        }
    }
}

void UnitNode::clear()
{
    if (_body->node) {
        _body->node->removeFromParent();
        _body->node = nullptr;
        _animation = nullptr;
        _baseParams->scale = 1.0f;
        _baseParams->speed = 1.0f;
    }
    
    if (_equipment->node) {
        _equipment->node->removeFromParent();
        _equipment->node = nullptr;
    }
    
    notifyRemoveShadow();
}

void UnitNode::scaleAnimationNode(AnimationNode* an, float scale)
{
    if (an) {
        this->scale(an->node, scale);
        this->scale(an->shadow, scale);
    }
}

void UnitNode::updateAnimationParams()
{
    auto sc(thisSkillClass());
    float scale(1.0f);
    float speed(1.0f);
    
    if (_extraParams.find(sc) != end(_extraParams)) {
        const auto& ap = _extraParams.at(sc);
        scale = ap->scale;
        speed = ap->speed;
    }
    
    scale *= _baseParams->scale;
    speed *= _baseParams->speed;
    
    if (_body->node) {
        scaleAnimationNode(_body, scale);
        scaleAnimationNode(_equipment, scale);
        
        if (_speedScheduler) {
            if (speed != 1.0f) {
                _speedScheduler->setTimeScale(speed);
            } else if (_speedScheduler->getTimeScale() != 1.0f) {
                _speedScheduler->setTimeScale(1.0f);
            }
        }
    }
}

#pragma mark - effects
Node* UnitNode::addEffect(const string& file)
{
    return addAnimation(file, false, nullptr, false, SpellConfigData::kNone, SpellConfigData::kBody);
}

Node* UnitNode::addAnimation(const string& file,
                             bool loop,
                             const function<void()>& callback,
                             bool scale,
                             const SpellConfigData::Direction& direction,
                             const SpellConfigData::Position& position)
{
    if (!_sprite || file.empty()) {
        return nullptr;
    }
    
    auto effect = CocosUtils::playAnimation(file, DEFAULT_FRAME_DELAY, loop, 0, -1, [callback](Node* sender) {
        sender->removeFromParent();
        if (callback) {
            callback();
        }
    });
    if (effect) {
        // 1. add effect
        addChild(effect, zOrder_top);
        
        // 2. adjust effect
        if (".plist" != FileUtils::getInstance()->getFileExtension(file)) {
            adjustEffect(effect, scale, direction, position);
        }
    }
    
    return effect;
}

void UnitNode::adjustEffect(Node* effect,
                            bool scale,
                            const SpellConfigData::Direction& direction,
                            const SpellConfigData::Position& position)
{
//    if (effect) {
//        bool flip(false);
//        if (SpellConfigData::kNone != direction) {
//            const bool isFaceRight(_configData->getAnimationType() == "pvr" ? _needToFlip : (_needToFlip ^ _configData->isFaceRight()));
//            flip = (isFaceRight ^ (SpellConfigData::kRight == direction));
//        }
//        
//        if (flip ^ (_needToFlip != _isFlipped)) {
//            flipX(effect);
//        }
//        
//        // set position
//        auto baseScale = _baseParams->scale;
//        if (SpellConfigData::kFoot == position) {
//            effect->setPosition(_configData->getFootEffectPosition());
//            if (scale) {
//                this->scale(effect, _configData->getFootEffectScaleX() * baseScale, _configData->getFootEffectScaleY() * baseScale);
//            } else {
//                this->scale(effect, _configData->getFootEffectScaleX(), _configData->getFootEffectScaleY());
//            }
//            effect->setLocalZOrder(zOrder_bottom);
//        } else if (SpellConfigData::kBody == position) {
//            effect->setPosition(_sprite->getPosition());
//            if (scale) {
//                this->scale(effect, baseScale);
//            }
//        } else if (SpellConfigData::kHead == position) {
//            effect->setPosition(getHPBarPosition());
//            if (scale) {
//                this->scale(effect, baseScale);
//            }
//        }
//    }
}

void UnitNode::addSwordEffect()
{
//    const auto& file = StringUtils::format(_configData->getSwordEffect().c_str(), _direction);
//    if (file.length() > 0) {
//        addEffect(file);
//    }
}

#pragma mark bufs
void UnitNode::addBuf(const string& name)
{
    if (_sprite && _bufs.find(name) == end(_bufs)) {
        auto data = DataManager::getInstance()->getSpellConfigData(name);
        if (data) {
//            const auto& files = data->getReceiverResourceNames();
//            if (files.size() > 0) {
//                const auto& file = files.at(0);
//                auto buf = addAnimation(file, true, nullptr, true, data->getReceiverSpellDirection(), data->getReceiverSpellPosition());
//                if (buf) {
//                    _bufs.insert(make_pair(name, buf));
//                }
//            }
        }
    }
}

void UnitNode::removeBuf(const string& name)
{
    if (_bufs.find(name) != _bufs.end()) {
        auto buf = _bufs.at(name);
        if (buf) {
            buf->removeFromParent();
        }
        _bufs.erase(name);
    }
}

void UnitNode::removeAllBufs()
{
    for (auto iter = _bufs.begin(); iter != _bufs.end(); ++iter) {
        auto buf = iter->second;
        if (buf) {
            buf->removeFromParent();
        }
    }
    
    _bufs.clear();
}

void UnitNode::updateBufs()
{
    auto last = _bufNames;
    
    _bufNames.clear();
    for (int i = 0; i < _unit->getBuffCount(); ++i) {
        const auto& bufName = _unit->getBuff(i)->getBuffType()->getRenderKey();
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
    
    auto cnt = added.size();
    if (cnt > 0) {
        // re-calculate the buf scale
        for (auto iter = added.begin(); iter != added.end(); ++iter) {
            const auto& name = *iter;
            auto data = DataManager::getInstance()->getSpellConfigData(name);
            if (data) {
                {
//                    const auto& rates = data->getReceiverSpeedRates();
//                    for (auto iter = rates.begin(); iter != rates.end(); ++iter) {
//                        auto sc = iter->first;
//                        auto rate = iter->second;
//                        if (_extraParams.find(sc) != end(_extraParams)) {
//                            const auto& ap = _extraParams.at(sc);
//                            ap->speed = rate + ap->speed;
//                        } else {
//                            auto ap = new (nothrow) AnimationParameter(nullptr);
//                            ap->speed = rate;
//                            _extraParams.insert(make_pair(sc, ap));
//                        }
//                    }
//                }
//                
//                {
//                    const auto& rates = data->getReceiverVolumeRates();
//                    for (auto iter = rates.begin(); iter != rates.end(); ++iter) {
//                        auto sc = iter->first;
//                        auto rate = iter->second;
//                        if (_extraParams.find(sc) != end(_extraParams)) {
//                            const auto& ap = _extraParams.at(sc);
//                            ap->scale = rate + ap->scale;
//                        } else {
//                            auto ap = new (nothrow) AnimationParameter(nullptr);
//                            ap->scale = rate;
//                            _extraParams.insert(make_pair(sc, ap));
//                        }
//                    }
                }
                
                addBuf(*iter);
            }
        }
        
        // calculate average
        {
            for (auto iter = begin(_extraParams); iter != end(_extraParams); ++iter) {
                const auto& ap = iter->second;
                ap->scale = ap->scale / cnt;
                ap->speed = ap->speed / cnt;
            }
        }
    }
    
    updateAnimationParams();
}

void UnitNode::updateFeatures(const Game* game)
{
    list<Unit::EventLog> eventLogs;
    for (auto iter = eventLogs.begin(); iter != eventLogs.end(); ++iter) {
        const auto& log = *iter;
        auto type = log._type;
        if (type == Unit::kEventLogType_DamageOutput ||
            type == Unit::kEventLogType_DamageInupt) {
            auto dn = log._damageNature;
            if (dn == kDamageNature_Hurt) {
                // TODO:
            } else if (dn == kDamageNature_Heal) {
                static const string file("huifu-2.csb");
                addEffect(file);
            }
        } else if (type == Unit::kEventLogType_FeatureTakeEffect) {
            auto ft = log._featureType;
            if (ft) {
                const auto& renderKey = ft->getRenderKey();
                auto data = DataManager::getInstance()->getSpellConfigData(renderKey);
                if (data) {
//                    const auto& files = data->getReceiverResourceNames();
//                    if (files.size() > 0) {
//                        const auto& file = files.at(0);
//                        addAnimation(file, false, nullptr, true, data->getReceiverSpellDirection(), SpellConfigData::kBody);
//                    }
                }
            }
        } else if (type == Unit::kEventLogType_ResourceOutput) {
            static const string rollHintScheduleKeyPrefix("ROLL_HINT_SCHEDULE_KEY_");
            static const string rollHintScheduleKeySplitor("_");
            
            if (log._factionIndex == game->getThisFactionIndex()) {
                float delay = 0.3f;
                int index = 0;
                for (auto iter = log._resources.begin();
                     iter != log._resources.end();
                     ++iter) {
                    auto resource = iter->first;
                    auto amount = iter->second;
                    auto scheduleKey = rollHintScheduleKeyPrefix
                    + UnderWorld::Core::UnderWorldCoreUtils::to_string(_unit->getId())
                    + rollHintScheduleKeySplitor
                    + UnderWorldCoreUtils::to_string(++_rollHintCounter);
                    rollHintResource(resource, amount, delay * index);
                    ++index;
                }
            }
        }
    }
}

#pragma mark hp bar
void UnitNode::addHPBar()
{
    if (!_hpBar && _unit && _sprite) {
        _hpBar = DisplayBar::create("GameImages/test/ui_blood.png", "GameImages/test/ui_blood_2.png");
        _hpBar->setPosition(getHPBarPosition());
        addChild(_hpBar, zOrder_top);
        if (_configData) {
            scale(_hpBar, _configData->getHpBarScaleX(), 1.0f);
        }
    }
}

void UnitNode::updateHPBar()
{
    if (_hpBar && _unit) {
        _hpBar->setPercentage(getHpPercentage());
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
        const auto& size = _sprite->getContentSize();
        const auto& pos = _sprite->getPosition();
        float offsetX(0);
        float offsetY(0);
        if (_configData) {
            offsetX = _configData->getHpBarPosX();
            offsetY = _configData->getHpBarPosY();
        }
        
        auto position(pos + Point(offsetX, size.height / 2 + offsetY));
        position = convertToNodeSpace(_sprite->getParent()->convertToWorldSpace(position));
        return position;
    }
    
    return Point::ZERO;
}

#pragma mark shadow
Node* UnitNode::createShadow(const string& file, bool flip, int startIdx)
{
    Node* shadow(nullptr);
    Point offset(Point::ZERO);
    
    if (_configData->getAnimationType() == UnitAnimationType::PVR) {
        shadow = CocosUtils::getAnimationNode(file, startIdx);
    } else {
        shadow = Sprite::create(file);
        //offset = _configData->getFootEffectPosition();
    }
    
    if (flip) {
        flipX(shadow);
    }
    
    if (_observer) {
        _observer->onUnitNodeCreateShadow(this, shadow, offset);
    }
    
    return shadow;
}

void UnitNode::createShadow(AnimationNode** an, bool flip, int startIdx)
{
    if (an && (*an)) {
        const auto& nodeFile((*an)->nodeFile);
        if (!nodeFile.empty()) {
            auto file = getShadowFile(nodeFile, flip);
            (*an)->shadowFile = file;
            if (!file.empty()) {
                (*an)->shadow = createShadow(file, flip, startIdx);
            }
        }
    }
}

void UnitNode::notifyRemoveShadow()
{
    if (_observer) {
        _observer->onUnitNodeRemoveShadow(this);
    }
    
    _body->shadow = nullptr;
    _equipment->shadow = nullptr;
}

#pragma mark equipment
void UnitNode::createEquipment(AnimationNode** an, const string& file, bool flip, int startIdx)
{
    if (an && (*an) && !file.empty()) {
        auto nodeFile = getEquipmentFile(file, flip);
        (*an)->nodeFile = nodeFile;
        if (!nodeFile.empty()) {
            auto node = CocosUtils::getAnimationNode(nodeFile, startIdx);
            if (flip) {
                flipX(node);
            }
            addChild(node);
            (*an)->node = node;
        }
    }
}

#pragma mark hint
void UnitNode::rollHintResource(const string& resource,
                                float amount,
                                float delay)
{
    auto hintNode = Node::create();
    Node* iconNode(nullptr);
    if (resource == RES_NAME_GOLD) {
        iconNode = Sprite::create(StringUtils::format("GameImages/resources/icon_%dB.png", ::ResourceType::Gold));
    } else if (resource == RES_NAME_WOOD) {
        iconNode = Sprite::create(StringUtils::format("GameImages/resources/icon_%dB.png", ::ResourceType::Wood));
    }
    assert(iconNode);
    iconNode->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
    iconNode->setScale(0.5f);
    
    auto amountNode = CocosUtils::create10x25Number("+" + UnderWorldCoreUtils::to_string(amount));
    amountNode->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    
    hintNode->addChild(iconNode);
    hintNode->addChild(amountNode);
    rollHintNode(hintNode, delay);
}

void UnitNode::rollHintNode(Node* hintNode, float delay)
{
    if (!hintNode) return;
    hintNode->setCascadeOpacityEnabled(true);
    hintNode->setPosition(getPosition());
    hintNode->setVisible(false);
    getParent()->addChild(hintNode);
    static float duration(2.0f);
    auto fadeTo = Sequence::create(DelayTime::create(1.5f), FadeTo::create(duration - 1.5f, 0), nullptr);
    auto moveBy = MoveBy::create(duration, Point(0.f, 100.f));
    hintNode->runAction(Sequence::create(DelayTime::create(delay),
                                         CallFunc::create([hintNode](){ hintNode->setVisible(true); }),
                                         Spawn::create(fadeTo, moveBy, nullptr),
                                         RemoveSelf::create(), nullptr));
}
