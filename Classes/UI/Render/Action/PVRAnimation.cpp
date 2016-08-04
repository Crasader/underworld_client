//
//  PVRAnimation.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/8/1.
//
//
#include "Global.h"
#include "PVRAnimation.h"
#include "CocosUtils.h"

const float PVRAnimation::PVR_ANIMATION_FRAME_DELAY = 1.f / 12.f;

PVRAnimation* PVRAnimation::create(const std::string& folder) {
    PVRAnimation* ret = new (std::nothrow) PVRAnimation();
    if (ret && ret->init(folder)) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}


PVRAnimation::~PVRAnimation() {
    CC_SAFE_RELEASE(_animate);
    CC_SAFE_RELEASE(_playingAnimate);
}


bool PVRAnimation::init(const std::string& folder) {
    // 1. clean up
    _frames.clear();
    CC_SAFE_RELEASE(_animate);
    CC_SAFE_RELEASE(_playingAnimate);
    _animate = _playingAnimate = nullptr;
    
    // 2. get frames
    for (int i = 0; true; ++i) {
        cocos2d::SpriteFrame* frame = dynamic_cast<cocos2d::SpriteFrame*>(CocosUtils::getAnimationNode(folder, i));
        
        if (frame) _frames.pushBack(frame);
        else break;
    }
    
    if (_frames.empty()) return false;
    
    auto animation = cocos2d::Animation::createWithSpriteFrames(_frames);
    animation->setDelayPerUnit(PVR_ANIMATION_FRAME_DELAY);
    animation->setRestoreOriginalFrame(false);
    _animate = cocos2d::Animate::create(animation);
    _animate->retain();
    
    return true;
}

void PVRAnimation::play(cocos2d::Node* node, bool loop,
    int beginFrameIndex, const std::function<void()>& callback) {
    if (!node) return;
    
    beginFrameIndex = m_clampi(beginFrameIndex, 0, (int)_frames.size());
    if (loop) beginFrameIndex = 0;
    
    cocos2d::Vector<cocos2d::SpriteFrame*> playFrames;
    for (int i = beginFrameIndex; i < _frames.size(); ++i) {
        playFrames.pushBack(_frames.at(i));
    }
    
    if (!playFrames.empty()) {
        
        _beginFrameIndex = beginFrameIndex;
        
        node->stopAllActions();
        auto animation = cocos2d::Animation::createWithSpriteFrames(playFrames);
        animation->setDelayPerUnit(PVR_ANIMATION_FRAME_DELAY);
        animation->setRestoreOriginalFrame(false);
        
        cocos2d::Action* action(nullptr);
        CC_SAFE_RELEASE(_playingAnimate);
        _playingAnimate = cocos2d::Animate::create(animation);
        _playingAnimate->retain();
        if (loop) {
            action = cocos2d::RepeatForever::create(_playingAnimate);
        } else {
            action = cocos2d::Sequence::create(_playingAnimate, cocos2d::CallFunc::create(callback), nullptr) ;
        }
        
        node->runAction(action);
    }
    
}
cocos2d::Node* PVRAnimation::buildNode() const {
    return cocos2d::Sprite::createWithSpriteFrame(_frames.front());
}


int PVRAnimation::getCurrentFrame() const {
    return _playingAnimate ? (_beginFrameIndex + _playingAnimate->getCurrentFrameIndex()) : 0;
}

float PVRAnimation::getDuration() const {
    return _animate->getDuration();
}

PVRAnimation::PVRAnimation()
: _animate(nullptr)
, _playingAnimate(nullptr)
, _beginFrameIndex(0) {
}