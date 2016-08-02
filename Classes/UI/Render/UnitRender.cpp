//
//  UnitRender.cpp
//  Underworld_Client
//
//  Created by Mac on 16/7/18.
//
//

#include "UnitRender.h"
#include "Global.h"
#include "WorldObject.h"
#include "DataManager.h"
#include "WorldRender.h"
#include "DisplayBar.h"
#include "Unit.h"
#include "World.h"
#include "Game.h"
#include "URConfigData.h"
#include "SpellConfigData.h"
#include "UWRender.h"
#include "Skill.h"
#include "CocosUtils.h"

namespace UnderWorld{ namespace Core{
    
void BodyAnimationPoseBundle::specificDurationAndBeginTime(float duration, float beginTime) {
    _playDuration = duration;
    _playBeginTime = beginTime;
    _playDurationScale = 1.f;
    
    _specificScale = false;
}

void BodyAnimationPoseBundle::specificDurationScale(float scale) {
    _playDurationScale = scale;
    
    _specificScale = true;
}
    
void BuffAnimation::increaseContributors() {
    ++_contributors;
}

void BuffAnimation::descreaseContributors() {
    if (_contributors > 0) --_contributors;
}
    
const int UnitRender::IN_MAIN_BODY_NODE_ZORDER = 1;
const int UnitRender::IN_MAIN_HP_BAR_ZORDER = 2;
const int UnitRender::IN_MAIN_EFFECT_ZORDER = 3;
    
const float UnitRender::HEALTHY_HP_THRESHOLD = .3f;
const int UnitRender::MAX_HP_PERCENT = 100;
const std::string UnitRender::HEAL_EFFECT_RENDER_KEY = "";
const std::string UnitRender::ROLL_SCHEDULE_KEY_PREFIX = "unit_roll_schedule_prefix";
const float UnitRender::ROLL_NEXT_DELAY_IN_SECOND = .2f;
const float UnitRender::DUARTION_SCALE_MAX = 100.f;
    
UnitRender::UnitRender()
: _unit(nullptr)
, _unitType(nullptr)
, _configData(nullptr)
, _mainNode(nullptr)
, _groundNode(nullptr)
, _unitView(nullptr)
, _hpBar(nullptr) {
}

UnitRender::~UnitRender() {
    CC_SAFE_RELEASE(_mainNode);
    CC_SAFE_RELEASE(_groundNode);
    CC_SAFE_DELETE(_unitView);
    while (!_rollNodes.empty()) {
        CC_SAFE_RELEASE(_rollNodes.front());
        _rollNodes.pop();
    }
    if (_unit && !isObjectReleased()) _unit->removeUnitObserver(this);
}

bool UnitRender::init(const WorldObject *object, WorldRender *worldRender) {
    if (!AbstractWorldObjectRender::init(object, worldRender)) return false;
    
    // init data
    _unit = dynamic_cast<const Unit*>(object);
    if (!_unit) return false;
    
    _unitType = _unit->getUnitBase().getUnitType();
    _unitRenderKey = _unit->getUnitBase().getRenderKey();
    _configData = DataManager::getInstance()->getURConfigData(_unitRenderKey);
    if (!_unitType || !_configData) return false;
    
    // init events
    _events.clear();
    _usefulAddBuffLogs.clear();
    _usefuleliminateBuffLogs.clear();
    _usefulHoldAuraLogs.clear();
    _usefulUnholdAuraLogs.clear();
    _featureLogs.clear();
    if (_unit) _unit->addUnitObserver(this);
    
    //init status
    updateStatus();

    _renderBufs.clear();
    _bufAnimations.clear();
    
    // init cocos
    if (!_mainNode) {
        _mainNode = cocos2d::Node::create();
        _mainNode->retain();
    }
    _mainNode->removeAllChildren();
    if (!_groundNode) {
        _groundNode = cocos2d::Node::create();
        _groundNode->retain();
    }
    _groundNode->removeAllChildren();
    _unitView = nullptr;
    _hpBar = nullptr;
    while (!_rollNodes.empty()) {
        CC_SAFE_RELEASE(_rollNodes.front());
        _rollNodes.pop();
    }
    _rollScheduleKey = ROLL_SCHEDULE_KEY_PREFIX + UnderWorldCoreUtils::to_string(_unit->getId());
    
    // init render
    renderPosition(true);
    initRenderSkill();
    renderHp(true);
    initBuffAndAura();
    
    return true;
}

void UnitRender::render() {
    AbstractWorldObjectRender::render();
    
    if (AbstractWorldObjectRender::isObjectReleased()) {
        //handle object release
        if (!_unitView || _unitView->getBodyAnimationPose() != UnitAnimationPose::Killed) {
            _worldRender->deleteWorldObjectRender(this);
        }
        return;
    }
    
    // handle unit events
    handleEvents();
    
    // update with unit's status
    updateStatus();
    
    // render all part's of unit
    renderPosition();
    renderSkill();
    renderHp();
    renderBuffAndAura();
    renderEffects();
    
    // clean all input event
    // cause of don't want to copy event, keep the log list until render over
    _events.clear();
}

void UnitRender::attach2WorldRender(WorldRender* worldRender) {
    if (worldRender->getWorldContainer()) {
        if (_mainNode) worldRender->getWorldContainer()->addChild(_mainNode);
        if (_groundNode) worldRender->getWorldContainer()->addChild(_groundNode);
    }
}

void UnitRender::dettachFromWorldRender(WorldRender* worldRender) {
    if (worldRender->getWorldContainer()) {
        if (_mainNode) worldRender->getWorldContainer()->removeChild(_mainNode);
        if (_groundNode) worldRender->getWorldContainer()->removeChild(_groundNode);
    }
}

void UnitRender::onNotifyUnitEvents(const std::vector<Unit::EventLog>& events) {
    for (int i = 0; i < events.size(); ++i) {
        _events.push_back(events[i]);
    }
}
    
cocos2d::Node* UnitRender::addEffect(const std::string &renderKey, bool loop) {
    cocos2d::Node* ret = nullptr;
    
    const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(renderKey);
    
    // create node
    do {
        // check data
        if (!data) break;
        
        // check file
        const vector<string>& files = data->getReceiverResourceNames();
        if (files.size() <= 0 || files.at(0).empty()) break;
        
        const std::string& file = files.at(0);
        
        //check file name
        const size_t found = file.find_last_of(".");
        if (found == string::npos) break;
        
        const string& suffix = file.substr(found + 1);
        if ("csb" == suffix) {
            function<void(Node*)> callback = nullptr;
            if (!loop) {
                callback = [](Node* sender) { if (sender) sender->removeFromParent(); };
            }
            ret = CocosUtils::playCSBAnimation(file, loop, 0, callback);
        } else if ("plist" == suffix) {
            ParticleSystemQuad *particle = ParticleSystemQuad::create(file);
            if (particle) {
                if (!loop) particle->setAutoRemoveOnFinish(true);
                ret = particle;
            }
        }

    } while (0);
    
    // attach node
    if (ret) {
        //TDOO : position
        _mainNode->addChild(ret, IN_MAIN_EFFECT_ZORDER);
    }
    
    return ret;
}

void UnitRender::handleEvents() {
    for (int i = 0; i < _events.size(); ++i) {
        if (_events[i]._type == Unit::kEventLogType_AddBuff) {
            if (_usefuleliminateBuffLogs.find(_events[i]._buffId) == _usefuleliminateBuffLogs.end()) {
                _usefulAddBuffLogs.insert(std::make_pair(_events[i]._buffId, i));
            }
            
        } else if (_events[i]._type == Unit::kEventLogType_EliminateBuff) {
            _usefuleliminateBuffLogs.insert(std::make_pair(_events[i]._buffId, i));
            auto iter = _usefulAddBuffLogs.find(_events[i]._buffId);
            if (iter != _usefulAddBuffLogs.end()) _usefulAddBuffLogs.erase(iter);
            
        } else if (_events[i]._type == Unit::kEventLogType_HoldAura) {
            _usefulHoldAuraLogs.insert(std::make_pair(_events[i]._auraId, i));
            auto iter = _usefulUnholdAuraLogs.find(_events[i]._auraId);
            if (iter != _usefulUnholdAuraLogs.end()) _usefulUnholdAuraLogs.erase(iter);
            
        } else if (_events[i]._type == Unit::kEventLogType_unHoldAura) {
            _usefulUnholdAuraLogs.insert(std::make_pair(_events[i]._auraId, i));
            auto iter = _usefulHoldAuraLogs.find(_events[i]._auraId);
            if (iter != _usefulHoldAuraLogs.end()) _usefulHoldAuraLogs.erase(iter);
            
        } else if (_events[i]._type == Unit::kEventLogType_FeatureTakeEffect
            || _events[i]._type == Unit::kEventLogType_ResourceOutput) {
            _featureLogs.push_back(i);
        } else if (_events[i]._type == Unit::kEventLogType_DamageInupt
            && _events[i]._damageNature == kDamageNature_Heal) {
            _featureLogs.push_back(i);
        }
    }
}

void UnitRender::updateStatus() {
    _lastStatus = _status;
    _status._position = AbstractWorldObjectRender::getPos();
    
    if (!_unit) return;
    
    _status._field = _unit->getCurrentField();
    _status._direction = _unit->getDirection();
    _status._hp = _unit->getHp();
    if (_unit->getHp() == _unit->getUnitBase().getMaxHp()) {
        _status._hpPercent = MAX_HP_PERCENT;
    } else {
        _status._hpPercent = MAX_HP_PERCENT * (((float)_unit->getHp()) / ((float)_unit->getUnitBase().getMaxHp()));
    }
    _status._skill = _unit->getCurrentSkill();
    _status._isAlive = _unit->isAlive();
    _status._isHealthy = _status._hpPercent >= HEALTHY_HP_THRESHOLD;
    _status._factionIndex = _unit->getBelongFaction()->getFactionIndex();
}

void UnitRender::renderPosition(bool init) {
    if (!init &&_status._position == _lastStatus._position
        && _status._field == _lastStatus._field) {
        //position not change at all, need not render position again
        return;
    }
    
    cocos2d::Vec2 shadowPoint =
        _worldRender->worldCoordinate2CocosPoint(_unit->getCenterPos(),
            WorldRender::RenderLayer::Shadow);
    int shadowZorder = _worldRender->worldCoordinate2Zorder(_unit->getCenterPos(),
        WorldRender::RenderLayer::Shadow);
    _groundNode->setPosition(shadowPoint);
    _groundNode->setLocalZOrder(shadowZorder);
    
    WorldRender::RenderLayer mainNodeLayer(WorldRender::RenderLayer::Land);
    if (_unit->getCurrentField() == kFieldType_Land) {
        mainNodeLayer = WorldRender::RenderLayer::Land;
    } else if (_unit->getCurrentField() == kFieldType_Air) {
        mainNodeLayer = WorldRender::RenderLayer::Air;
    }
    cocos2d::Vec2 mainPoint = _worldRender->worldCoordinate2CocosPoint(_unit->getCenterPos(), mainNodeLayer);
    //use unit's rightUp pos to calculate zorder, overlap will cover unit
    int size = _unit->getUnitBase().getSize();
    int mainZorder = _worldRender->worldCoordinate2Zorder(_unit->getPos() + Coordinate32(size, size), mainNodeLayer);
    _mainNode->setPosition(mainPoint);
    _mainNode->setLocalZOrder(mainZorder);
}

void UnitRender::renderHp(bool init) {
    if (!init && _status._hp == _lastStatus._hp
        && _status._hpPercent == _lastStatus._hpPercent
        && _status._isAlive == _lastStatus._isAlive) {
        // hp not change
        return;
    }
    
    bool justCreatedHpbar = false;
    if (_status._hpPercent != MAX_HP_PERCENT && !_hpBar) {
        std::string background, foreground;
        getHpBarFiles(this, background, foreground);
        _hpBar = DisplayBar::create(background, foreground);
        justCreatedHpbar = true;
        if (_hpBar) {
            _hpBar->setPosition(calculateHpBarPosition());
            _mainNode->addChild(_hpBar, IN_MAIN_HP_BAR_ZORDER);
        }
    }
    
    if (_hpBar) {
        _hpBar->setPercentage(_status._hpPercent);
        
        bool hpBarVisible = _status._isAlive
            && (_unitType->getUnitClass() == kUnitClass_Hero || _status._hpPercent < MAX_HP_PERCENT);
        _hpBar->setVisible(hpBarVisible);
        
        if (!justCreatedHpbar && _status._factionIndex != _lastStatus._factionIndex) {
            std::string unused, foreground;
            getHpBarFiles(this, unused, foreground);
            if (foreground != _hpBar->getFgFile()) {
                _hpBar->setForeground(foreground);
            }
        }
    }
}

void UnitRender::renderSkill() {
    
    BodyAnimationPoseBundle currentPoseBundle;
    getCurrentPoseBundle(currentPoseBundle);
    
    BodyAnimationPoseBundle targetPoseBundle;
    needRebuildBodyAnim(currentPoseBundle, targetPoseBundle);
    
    if (targetPoseBundle.getPose() != UnitAnimationPose::None) {    // rebuild body anim
        buildAndPlayBodyAnimWithCurrentStatus(targetPoseBundle);
    } else {                  // tune body anim speed
        if (_unitView->getBodyAnimationPose() == UnitAnimationPose::Move) {
            if (_status._skill) {
                const MoveSkill* moveSkill = dynamic_cast<const MoveSkill*>(_status._skill);
                const MoveSkillType* mst = dynamic_cast<const MoveSkillType*>(_status._skill->getSkillType());
                if (moveSkill && mst) {
                    float scale = mst->getSpeed() == 0 ? 1.f : ((float)moveSkill->getSpeed() / mst->getSpeed());
                    _unitView->setDurationScale(scale);
                }
            }
        }
    }
    
    //TODO: game fast-forward & set global duration scale
}
    
void UnitRender::renderBuffAndAura() {
    /** render buf */
    for (auto iter = _usefulAddBuffLogs.begin(); iter != _usefulAddBuffLogs.end(); ++iter) {
        Unit::EventLog& addBufLog = _events[iter->second];
        if (addBufLog._buff) renderBuf(addBufLog._buff);
    }
    _usefulAddBuffLogs.clear();
    for (auto iter = _usefuleliminateBuffLogs.begin();
         iter != _usefuleliminateBuffLogs.end();
         ++iter) {
        Unit::EventLog& eliminateLog = _events[iter->second];
        stopRenderBuf(eliminateLog._buffId);
    }
    _usefuleliminateBuffLogs.clear();
    
    /** TODO: render aura */
    
    _usefulHoldAuraLogs.clear();
    _usefulUnholdAuraLogs.clear();
    
}
    
void UnitRender::renderEffects() {
    static frame_t delayThresholdInFrame = GameConstants::second2Frame(WorldRender::EFFECT_RENDER_DELAY_THRESHOLD_IN_SECOND);
    
    frame_t currentFrame = _worldRender->getWorld()->getClock()->getFrameCount();
    bool healed = false;
    for (int i = 0; i < _featureLogs.size(); ++i) {
        const Unit::EventLog& log = _events[i];
        
        // ignore too old feature
        if (currentFrame - log._frame > delayThresholdInFrame) continue;
            
        if (log._type == Unit::kEventLogType_FeatureTakeEffect) {
            if (log._featureType && !log._featureType->getRenderKey().empty()) {
                addEffect(log._featureType->getRenderKey(), false);
            }
        } else if (log._type == Unit::kEventLogType_ResourceOutput) {
            for (auto iter = log._resources.begin(); iter != log._resources.end(); ++iter) {
                if (iter->second <= 0 || iter->first.empty()) continue;
                cocos2d::Node* node = createResourceOuputHintNode(iter->first, iter->second);
                if (node) rollNode(node);
            }
        } else if (log._type == Unit::kEventLogType_DamageInupt) {
            if (log._damageNature == kDamageNature_Heal && log._damage > 0 && !healed) {
                healed = true;
            }
        }
    }
    
    if (healed) addEffect(HEAL_EFFECT_RENDER_KEY, false);
    _featureLogs.clear();
}
    
void UnitRender::initRenderSkill() {
    BodyAnimationPoseBundle currentPoseBundle;
    getCurrentPoseBundle(currentPoseBundle);
    buildAndPlayBodyAnimWithCurrentStatus(currentPoseBundle);
}
    
void UnitRender::initBuffAndAura() {
    if (!_unit) return;
    
    for (int i = 0; i < _unit->getBuffCount(); ++i) {
        const Buff* buf = _unit->getBuff(i);
        
        if (buf) renderBuf(buf);
    }
}
    
void UnitRender::renderBuf(const Buff* buf) {
    // check instance
    if (!buf || !buf->getBuffType()) return;
    
    // check already exists
    auto iter = _renderBufs.find(buf->getId());
    if (iter != _renderBufs.end()) return;
    
    // check renderKey
    std::string renderKey = buf->getBuffType()->getRenderKey();
    if (!renderKey.empty()) {
        auto iter = _bufAnimations.find(renderKey);
        if (iter != _bufAnimations.end()) {    // the kind of buf animation already exists
            iter->second.increaseContributors();
        } else {    // create buf animation
            iter = _bufAnimations.insert(std::make_pair(renderKey, BuffAnimation())).first;
            iter->second.increaseContributors();
            
            // create node
            cocos2d::Node* bufNode = addEffect(renderKey, true);
            if (bufNode) iter->second.setNode(bufNode);
        }
    }
    
    
    _renderBufs.insert(std::make_pair(buf->getId(), renderKey));
}
    
void UnitRender::stopRenderBuf(creatureid_t bufId) {
    auto iterA = _renderBufs.find(bufId);
    if (iterA == _renderBufs.end()) return;
    
    auto iterB = iterA->second.empty() ? _bufAnimations.end() : _bufAnimations.find(iterA->second);
    
    // remove animation
    if (iterB != _bufAnimations.end()) {
        iterB->second.descreaseContributors();
        if (iterB->second.getContributors() == 0) {
            if (iterB->second.getNode()) iterB->second.getNode()->removeFromParent();
            _bufAnimations.erase(iterB);
        }
    }
    
    // remove bufId record
    _renderBufs.erase(iterA);
    
}
    
void UnitRender::getCurrentPoseBundle(BodyAnimationPoseBundle& output) {
    output.setPose(UnitAnimationPose::Stand);
    output.specificDurationScale(1.f);
    
    const Skill* currentSkill = _status._skill;
    
    if (!currentSkill) return;
    
    SkillClass sc = currentSkill->getSkillClass();
    if (sc == kSkillClass_Attack) {
        if (currentSkill->getSkillState() == Skill::kSkillState_idle) {
            output.setPose(UnitAnimationPose::Stand);
        } else if (currentSkill->getSkillState() == Skill::kSkillState_prePerforming && currentSkill->getPrePerformProgress() == 0) {
            output.setPose(UnitAnimationPose::Stand);
        } else {
            output.setPose(UnitAnimationPose::Attack);
        }
    } else if (sc == kSkillClass_Cast) {
        if (currentSkill->getSkillState() == Skill::kSkillState_idle) {
            output.setPose(UnitAnimationPose::Stand);
        } else if (currentSkill->getSkillState() == Skill::kSkillState_prePerforming && currentSkill->getPrePerformProgress() == 0) {
            output.setPose(UnitAnimationPose::Stand);
        } else {
            output.setPose(UnitAnimationPose::Cast);
        }
    } else if (sc == kSkillClass_Die) {
        if (!_lastStatus._skill || _lastStatus._skill->getSkillClass() != kSkillClass_Die) {
            output.setPose(UnitAnimationPose::Killed);
        } else {
            output.setPose(UnitAnimationPose::Dead);
        }
    } else if (sc == kSkillClass_Move) {
        output.setPose(UnitAnimationPose::Move);
    } else if (sc == kSkillClass_Produce || sc == kSkillClass_Stop) {
        output.setPose(UnitAnimationPose::Stand);
    }
    
    if (output.getPose() == UnitAnimationPose::Attack || output.getPose() == UnitAnimationPose::Cast) {
        float playDuration = ((float)currentSkill->getCurrentTotalpreperformFrames()) / GameConstants::FRAME_PER_SEC;
        frame_t startTimeFrame = m_clampi(
            currentSkill->getCurrentTotalpreperformFrames() - currentSkill->getPrePerformProgress(),
            0,
            currentSkill->getCurrentTotalpreperformFrames());
        float playBeginTime = ((float)startTimeFrame) / GameConstants::FRAME_PER_SEC;
        output.specificDurationAndBeginTime(playDuration, playBeginTime);
    } else if (output.getPose() == UnitAnimationPose::Move) {
        const MoveSkill* moveSkill = dynamic_cast<const MoveSkill*>(currentSkill);
        const MoveSkillType* mst = dynamic_cast<const MoveSkillType*>(currentSkill->getSkillType());
        if (moveSkill && mst) {
            float scale = (mst->getSpeed() == 0) ? 1.f : ((float)moveSkill->getSpeed() / mst->getSpeed());
            output.specificDurationScale(scale);
        }
    }
    
}
    
void UnitRender::needRebuildBodyAnim(const BodyAnimationPoseBundle& curentPoseBundle, BodyAnimationPoseBundle& output) const {
    output.setPose(UnitAnimationPose::None);
    
    if (!_unitView) {
        output = curentPoseBundle;
    } else {
        bool oldPoseShouldPlayOver = false;
        UnitAnimationPose oldPose = _unitView->getBodyAnimationPose();
        
        if (curentPoseBundle.getPose() == UnitAnimationPose::Dead
            && oldPose == UnitAnimationPose::Killed) {
            oldPoseShouldPlayOver = true;
        } else if (curentPoseBundle.getPose() == UnitAnimationPose::Stand) {
            if (oldPose == UnitAnimationPose::Attack
                || oldPose == UnitAnimationPose::AttackPost
                || oldPose == UnitAnimationPose::Cast
                || oldPose == UnitAnimationPose::CastPost) {
                oldPoseShouldPlayOver = true;
            }
        }
        
        if (oldPoseShouldPlayOver) {
            output.setPose(oldPose);
            if (oldPose == UnitAnimationPose::Attack
                || oldPose == UnitAnimationPose::AttackPost
                || oldPose == UnitAnimationPose::Cast
                || oldPose == UnitAnimationPose::CastPost
                || oldPose == UnitAnimationPose::Killed) {
                if (_unitView->getDurationScale() == 0.f) {
                    output.specificDurationScale(1.f);
                } else {
                    output.specificDurationAndBeginTime(
                        _unitView->getAnimationDuration() / _unitView->getDurationScale(),
                        _unitView->getPlayTime() / _unitView->getDurationScale());

                }
                // never exec code
                output.specificDurationScale(1.f);
            }
        } else {
            output = curentPoseBundle;
        }
        
        assert(output.getPose() != UnitAnimationPose::None);
        
        //TODO:consider health accord to config daga
        if (output.getPose() == oldPose && _status._direction == _lastStatus._direction) {
            output.setPose(UnitAnimationPose::None);
        }
        
    }
}

void UnitRender::buildAndPlayBodyAnimWithCurrentStatus(BodyAnimationPoseBundle& poseBundle) {
    //1. create body animation for first time
    if (!_unitView) {
        _unitView = UnitView::create(_unitType);
        _unitView->retain();
        _mainNode->addChild(_unitView->getBodyNode(), IN_MAIN_BODY_NODE_ZORDER);
        _groundNode->addChild(_unitView->getShadowNode());
    }
    
    //2. destroy old animation & body node
    _unitView->stopAnimation();
    _unitView->destroyAnimation();
    
    //3. set animation params
    _unitView->setBodyAnimationPose(poseBundle.getPose());
    _unitView->setDirection(_status._direction);
    
    
    //4. build animtion & attach the node
    _unitView->buildAnimation();
    
    //5. set duration params
    if (poseBundle.isSpecificScale()) {
        _unitView->setDurationScale(poseBundle.getPlayDurationScale());
        _unitView->setBeginTime(0.f);
    } else {
        _unitView->setDurationScale(poseBundle.getPlayDuration() == 0.f
            ? DUARTION_SCALE_MAX
            : _unitView->getAnimationDuration() / poseBundle.getPlayDuration());
        _unitView->setBeginTime(poseBundle.getPlayBeginTime() * _unitView->getDurationScale());
    }
    
    //6. run animtion
    bool loop = poseBundle.getPose() == UnitAnimationPose::Stand
        || poseBundle.getPose() == UnitAnimationPose::Move
        || poseBundle.getPose() == UnitAnimationPose::Dead;
    std::function<void ()> callback = nullptr;
    if (!loop) {
        callback = std::bind(&UnitRender::bodyAnimationCallback, this);
    }
    
    if (!_unitView->runAnimation(loop, callback) && callback) {
        callback();
    }
}
    
void UnitRender::bodyAnimationCallback() {
    if (!_unitView) return;
    
    BodyAnimationPoseBundle bundle;
    bundle.setPose(UnitAnimationPose::Stand);
    
    if (_unitView->getBodyAnimationPose() == UnitAnimationPose::Attack) {
        bundle.setPose(UnitAnimationPose::AttackPost);
        bundle.specificDurationScale(_unitView->getDurationScale());
    } else if (_unitView->getBodyAnimationPose() == UnitAnimationPose::Cast) {
        bundle.setPose(UnitAnimationPose::CastPost);
        bundle.specificDurationScale(_unitView->getDurationScale());
    } else if (_unitView->getBodyAnimationPose() == UnitAnimationPose::Killed) {
        if (isObjectReleased()) {
            _worldRender->deleteWorldObjectRender(this);
        } else {
            if (_status._isAlive) bundle.setPose(UnitAnimationPose::Stand);
            else bundle.setPose(UnitAnimationPose::Dead);
        }
    } else {
        if (_status._isAlive) bundle.setPose(UnitAnimationPose::Stand);
        else bundle.setPose(UnitAnimationPose::Dead);
    }
    
    buildAndPlayBodyAnimWithCurrentStatus(bundle);
}
    
void UnitRender::rollNode(cocos2d::Node *node) {
    if (!node) return;
    
    if (_rollNodes.empty()) {
        cocos2d::Director::getInstance()->getScheduler()->schedule(std::bind(&UnitRender::rollNodeCallback, this, std::placeholders::_1), _mainNode, ROLL_NEXT_DELAY_IN_SECOND, false, _rollScheduleKey);
    }
    
    node->retain();
    _rollNodes.push(node);
}
    
void UnitRender::rollNodeCallback(float dt) {
    static float ROLL_DURATION(2.0f);
    static float FADE_DURATION(0.5f);
    static float ROLL_DIS_IN_PIXEL(100.f);
    
    if (!_rollNodes.empty()) {
        cocos2d::Node* rollNode = _rollNodes.front();
        _rollNodes.pop();
        if (rollNode) {
            rollNode->setCascadeOpacityEnabled(true);
            //TODO:: Position
            _mainNode->addChild(rollNode, IN_MAIN_EFFECT_ZORDER);
            
            Sequence* fadeTo = Sequence::create(DelayTime::create(ROLL_DURATION - FADE_DURATION), FadeTo::create(FADE_DURATION, 0), NULL);
            MoveBy* moveBy = MoveBy::create(ROLL_DURATION, Point(0.f, ROLL_DIS_IN_PIXEL));
            rollNode->runAction(Sequence::create(Spawn::create(fadeTo, moveBy, NULL),
                RemoveSelf::create(), NULL));
            rollNode->release();
        }
    }
    
    if (_rollNodes.empty()) {
        cocos2d::Director::getInstance()->getScheduler()->unschedule(_rollScheduleKey, _mainNode);
    }
    
}
    
cocos2d::Vec2 UnitRender::calculateHpBarPosition() {
    cocos2d::Vec2 ret;
    
    if (_unitView) {
        const cocos2d::Size& size = _unitView->getContentSize();
        const cocos2d::Vec2& position = _unitView->getPosition();
        ret += cocos2d::Vec2(position.x, position.y + size.height / 2);
    }
    
    float offsetX(0), offsetY(0);
    if (_configData) {
        offsetX = _configData->getHpBarPosX();
        offsetY = _configData->getHpBarPosY();
    }
    
    ret += cocos2d::Vec2(offsetX, offsetY);
    
    if (_unitView) {
        ret = _mainNode->convertToNodeSpace(_unitView->getParent()->convertToWorldSpace(ret));
    }

    return ret;
}
    
cocos2d::Node* UnitRender::createResourceOuputHintNode(const std::string& resourceName, int amount) {
    Node* hintNode = Node::create();
    if (!hintNode) return hintNode;
    
    Node* iconNode = nullptr;
    if (resourceName == RES_NAME_GOLD) {
        iconNode = Sprite::create("GameImages/resources/icon_" + UnderWorldCoreUtils::to_string((int)::ResourceType::Gold) + "B.png");
    } else if (resourceName == RES_NAME_WOOD) {
        iconNode = Sprite::create("GameImages/resources/icon_" + UnderWorldCoreUtils::to_string((int)::ResourceType::Wood) + "B.png");
    }
    
    assert(iconNode);
    if (iconNode) {
        iconNode->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        iconNode->setScale(0.5f);
        hintNode->addChild(iconNode);
    }
    
    auto amountNode = CocosUtils::create10x25Number("+" + UnderWorldCoreUtils::to_string(amount));
    if (amountNode) {
        amountNode->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        hintNode->addChild(amountNode);
    }
    
    return hintNode;
}
    
void UnitRender::getHpBarFiles(const UnitRender* ur, std::string& background, std::string& foreground) {
    
    if (!ur || !ur->_unitType || !ur->_unit) return;
    
    const bool big = ur->_unitType->getUnitClass() == kUnitClass_Building || ur->_unitType->getUnitClass() == kUnitClass_Core;
    const bool hero = ur->_unitType->getUnitClass() == kUnitClass_Hero;
    
    bool isAlly = ur->_unit->getBelongFaction()->getTeamIndex() == ur->_worldRender->getGameRender()->getGame()->getThisTeamIndex();
    if (big) {
        background.assign("GameImages/test/ui_blood_3.png");
        foreground.assign(isAlly ? "GameImages/test/ui_blood_5.png" : "GameImages/test/ui_blood_4.png");
    } else {
        if (hero) {
            background.assign("GameImages/test/ui_blood_hero_bg.png");
            foreground.assign("GameImages/test/ui_blood_hero.png");
        } else {
            background.assign("GameImages/test/ui_blood.png");
            foreground.assign(isAlly ? "GameImages/test/ui_blood_2.png" : "GameImages/test/ui_blood_1.png");
        }
    }
}


}}