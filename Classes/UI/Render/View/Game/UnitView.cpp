//
//  UnitView.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/7/31.
//
//

#include <iostream>
#include "cocostudio/CocoStudio.h"

#include "UnitView.h"
#include "DataManager.h"
#include "URConfigData.h"
#include "CocosUtils.h"
#include "PVRAnimation.h"
#include "CoreUtils.h"


namespace UnderWorld{ namespace Core{

UnitView* UnitView::create(const UnitType* unitType, bool isAlly) {
    UnitView* ret = new (std::nothrow) UnitView();
    if (ret && ret->init(unitType, isAlly)) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}
    
UnitView::~UnitView() {
    destroyAnimation();
    
    CC_SAFE_RELEASE(_shadowNode);
}
    
bool UnitView::init(const UnitType* unitType, bool isAlly) {
    if (!Node::init()) return false;
    
    destroyAnimation();
    
    /** init data */
    _unitType = unitType;
    if (_unitType) _configData = DataManager::getInstance()->getURConfigData(_unitType->getRenderKey());
    _isAlly = isAlly;
    _playCallback = nullptr;
    
    if (!_unitType || !_configData) return false;
    
    /** init status */
    _bodyAnimationFiles.clear();
    _shadowAnimationFiles.clear();
    
    _type = _configData->getAnimationType();
    _pose = UnitAnimationPose::Stand;
    _direction = Unit::kDirection_right;
    _durationScale = 1.f;
    _nodeScale = 1.f;
    _beginTime = 0.f;
    _flip = false;
    _playing = false;
    
    /** init cocos */
    if (!_shadowNode) _shadowNode = cocos2d::Node::create();
    if (_shadowNode) _shadowNode->retain();

    return true;
}
    
void UnitView::buildAnimation() {
    stopAnimation();
    destroyAnimation();
    
    getAnimationFiles(_type, _pose, _direction, _unitType, _configData, _bodyAnimationFiles, _shadowAnimationFiles, _isAlly);
    
    _flip = needToFlip(_direction, _configData);
    
    if (!_bodyAnimationFiles.empty()) {
        loadAnimation(_bodyAnimationFiles.front(), _type, _bodyAnimNode, _bodyAnimation);
        if (_bodyAnimNode) this->addChild(_bodyAnimNode);
        if (_bodyAnimation) _bodyAnimation->retain();
    }
    
    if (!_shadowAnimationFiles.empty() && _shadowNode) {
        loadAnimation(_shadowAnimationFiles.front(), _type, _shadowAnimNode, _shadowAnimation);
        if (_shadowAnimNode) _shadowNode->addChild(_shadowAnimNode);
        if (_shadowAnimation) _shadowAnimation->retain();
        
    }
    
    updateNodeScale();
}
bool UnitView::runAnimation(bool loop, const std::function<void ()>& callback) {
    if (_playing || !_bodyAnimNode || !_bodyAnimation) return false;
    _playCallback = callback;
    
    std::function<void ()> publicCallback = nullptr;
    if (!loop) {
        publicCallback = std::bind(&UnitView::playAnimationCallback, this);
    }
    
    //1. build schduler and actionManager
    if (!_animScheduler) {
        _animScheduler = new (std::nothrow) cocos2d::Scheduler();
        Director::getInstance()->getScheduler()->scheduleUpdate(_animScheduler, 0, false);
        if (!_actionManager) {
            _actionManager = new (std::nothrow) cocos2d::ActionManager();
            _animScheduler->scheduleUpdate(_actionManager, 0, false);
        }
    }
    
    //2. set action manager
    if (_actionManager) {
        if (_bodyAnimNode) _bodyAnimNode->setActionManager(_actionManager);
        if (_shadowAnimNode) _shadowAnimNode->setActionManager(_actionManager);
    }
    
    //3. update duration scale
    updateDurationScale();
    
    //4. play animation
    if (_type == UnitAnimationType::PVR) {
        PVRAnimation* bodyPVR = dynamic_cast<PVRAnimation*>(_bodyAnimation);
        if (bodyPVR && bodyPVR->getFrameCount() > 0) {
            int beginFrame = (bodyPVR->getDuration() == 0.f) ? 0 : (_beginTime / bodyPVR->getDuration()) * bodyPVR->getFrameCount();
            // don't do animation on last frame
            if (beginFrame == bodyPVR->getFrameCount()) --beginFrame;
            
            bodyPVR->play(_bodyAnimNode, loop, beginFrame, publicCallback);
            
            PVRAnimation* shadowPVR = dynamic_cast<PVRAnimation*>(_shadowAnimation);
            
            if (shadowPVR && _shadowAnimNode) {
                shadowPVR->play(_shadowAnimNode, loop, beginFrame, nullptr);
            }
            
            _playing = true;
        }
    } else if (_type == UnitAnimationType::CSB) {
        cocostudio::timeline::ActionTimeline* bodyTimeline = dynamic_cast<cocostudio::timeline::ActionTimeline*>(_bodyAnimation);
        if (bodyTimeline && bodyTimeline->getDuration() > 0) {
            int beginFrame = (int)(_beginTime * 60.f);
            // don't do animation on last frame
            if (beginFrame == bodyTimeline->getDuration()) --beginFrame;
            
            _bodyAnimNode->runAction(bodyTimeline);
            bodyTimeline->gotoFrameAndPlay(0, loop);
            if (!loop) bodyTimeline->setCurrentFrame(beginFrame);
            bodyTimeline->setLastFrameCallFunc(publicCallback);
            
            cocostudio::timeline::ActionTimeline* shadowTimeline = dynamic_cast<cocostudio::timeline::ActionTimeline*>(_shadowAnimation);
            if (shadowTimeline && _shadowAnimNode) {
                _shadowAnimNode->runAction(shadowTimeline);
                shadowTimeline->gotoFrameAndPlay(0, loop);
                if (!loop) shadowTimeline->setCurrentFrame(beginFrame);
                shadowTimeline->setLastFrameCallFunc(nullptr);
            }
            
            _playing = true;
        }
    }
    
    return _playing;
    
}
void UnitView::stopAnimation() {
    if (!_playing) return;
    
    if (_bodyAnimNode) _bodyAnimNode->stopAllActions();
    if (_shadowAnimNode) _shadowAnimNode->stopAllActions();
    
    _playing = false;
    
}
void UnitView::destroyAnimation() {
    stopAnimation();
    
    this->removeAllChildren();
    if (_shadowNode) _shadowNode->removeAllChildren();
    CC_SAFE_RELEASE(_bodyAnimation);
    CC_SAFE_RELEASE(_shadowAnimation);
    
    _bodyAnimNode = nullptr;
    _bodyAnimation = nullptr;
    _shadowAnimNode = nullptr;
    _shadowAnimation = nullptr;
    
    _bodyAnimationFiles.clear();
    _shadowAnimationFiles.clear();
}
    
float UnitView::getAnimationDuration() const {
    if (_bodyAnimation) {
        if (_type == UnitAnimationType::PVR) {
            PVRAnimation* pvr = dynamic_cast<PVRAnimation*>(_bodyAnimation);
            if (pvr) return pvr->getDuration();
        } else if (_type == UnitAnimationType::CSB) {
            cocostudio::timeline::ActionTimeline* timeline =
                dynamic_cast<cocostudio::timeline::ActionTimeline*>(_bodyAnimation);
            if (timeline) return timeline->getDuration() / 60.f;
        }
    }
    return 0.f;
}
    
float UnitView::getPlayTime() const {
    if (_bodyAnimation) {
        if (_type == UnitAnimationType::PVR) {
            PVRAnimation* pvr = dynamic_cast<PVRAnimation*>(_bodyAnimation);
            if (pvr) {
                if (pvr->getFrameCount() == 0)  return 0.f;
                return pvr->getCurrentFrame() * pvr->getDuration() / pvr->getFrameCount();
            
            }
        } else if (_type == UnitAnimationType::CSB) {
            cocostudio::timeline::ActionTimeline* timeline =
            dynamic_cast<cocostudio::timeline::ActionTimeline*>(_bodyAnimation);
            if (timeline) return timeline->getCurrentFrame() / 60.f;
        }
    }
    return 0.f;
}
    
void UnitView::setDurationScale(float durationScale) {
    _durationScale = durationScale;
    
    updateDurationScale();
    
}
    
void UnitView::setNodeScale(float nodeScale) {
    _nodeScale = nodeScale;
    
    updateNodeScale();
}
    
UnitView::UnitView()
: _unitType(nullptr)
, _configData(nullptr)
, _isAlly(true)
, _playCallback(nullptr)
, _type(UnitAnimationType::PVR)
, _pose(UnitAnimationPose::Stand)
, _direction(Unit::kDirection_right)
, _durationScale(1.f)
, _nodeScale(1.f)
, _beginTime(0.f)
, _flip(false)
, _playing(false)
, _shadowNode(nullptr)
, _bodyAnimNode(nullptr)
, _bodyAnimation(nullptr)
, _shadowAnimNode(nullptr)
, _shadowAnimation(nullptr)
, _animScheduler(nullptr)
, _actionManager(nullptr) {
    
};
    
void UnitView::loadAnimation(const std::string& file,
    UnitAnimationType type,
    cocos2d::Node*& animNodeOutput,
    cocos2d::Ref*& animationOutput) {
    if (type == UnitAnimationType::PVR) {
        PVRAnimation* pvr = PVRAnimation::create(file);
        if (pvr) {
            animationOutput = pvr;
            animNodeOutput = pvr->buildNode();
        }
    } else if (type == UnitAnimationType::CSB) {
        std::string fullPath = FileUtils::getInstance()->fullPathForFilename(file);
        if (FileUtils::getInstance()->isFileExist(fullPath)) {
            animNodeOutput = CSLoader::createNode(file);
            animationOutput = CSLoader::createTimeline(file);
        }
    }
}
    
void UnitView::playAnimationCallback() {
    stopAnimation();
    
    if (_playCallback) _playCallback();
}
    

void UnitView::updateNodeScale() {
    float scaleY = _nodeScale;
    if (_configData) scaleY *= _configData->getBodyScale();
    float scaleX = scaleY * ((scaleY > 0 && _flip) ? -1 : 1);
    this->setScale(scaleX, scaleY);
    if (_shadowNode) _shadowNode->setScale(scaleX, scaleY);
}
    
void UnitView::updateDurationScale() {
    if (_animScheduler) {
        _animScheduler->setTimeScale(_durationScale);
    }
}
    
void UnitView::getAnimationFiles(UnitAnimationType type,
    UnitAnimationPose pose,
    Unit::Direction direction,
    const UnitType* unitType,
    const URConfigData* configData,
    std::vector<std::string>& bodyAnimationOutput,
    std::vector<std::string>& shadowAnimationOutput,
    bool isAlly) {

    static std::vector<std::vector<std::string> > pose_files = {
        {"stand", "run", "attack", "attack_post", "skill", "skill_post", "killed", "dead", ""},
        {"standby", "run", "attack", "attack_post", "skill", "skill_post", "dead", "dead_body", ""}
    };
    
    if (!configData || !unitType) return;
    
    std::string bodyData;
    std::string shadowData;
    
    std::string resourcePrefix = configData->getNormalPrefix();
    std::string posePrefix = pose_files[(int)type][(int)pose];
    int bodyResId = getResourceId(type, direction, configData, false);
    int shadowResId = getResourceId(type, direction, configData, true);
    
    if (!resourcePrefix.empty() && !posePrefix.empty()) {
        if (type == UnitAnimationType::PVR) {
            if (unitType->getUnitClass() == kUnitClass_Core) {
                bodyData.assign(resourcePrefix + "/" + posePrefix + "/body/" + (isAlly ? "Blue/" : "Red/")  + UnderWorldCoreUtils::to_string(bodyResId));
            } else {
                bodyData.assign(resourcePrefix + "/" + posePrefix + "/body/" + UnderWorldCoreUtils::to_string(bodyResId));
            }
            shadowData.assign(resourcePrefix + "/" + posePrefix + "/shadows/" + UnderWorldCoreUtils::to_string(shadowResId));
        } else if (type == UnitAnimationType::CSB) {
            bodyData.assign(resourcePrefix + "-" + posePrefix + "-" + UnderWorldCoreUtils::to_string(bodyResId) + ".csb");
        }
    }
    
    if (!bodyData.empty()) bodyAnimationOutput.push_back(bodyData);
    if (!shadowData.empty()) shadowAnimationOutput.push_back(shadowData);
}
    
int UnitView::getResourceId(UnitAnimationType type, Unit::Direction direction, const URConfigData* configData, bool isShadow) {
    if (configData && !configData->isMultiDirection()) return 0;
        
    if (type == UnitAnimationType::PVR) {
        if (isShadow) {
            return ((int)direction) + 1;
        } else {
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
        }
    } else if (type == UnitAnimationType::CSB) {
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
    
    return 0;
}
    
bool UnitView::needToFlip(Unit::Direction direction, const URConfigData* configData) {
    bool flip(false);
    
    if (configData && !configData->isMultiDirection()) return flip;
    
    const bool isFaceRight(configData ? configData->isFaceRight() : false);

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
    
    return flip;
}


}}