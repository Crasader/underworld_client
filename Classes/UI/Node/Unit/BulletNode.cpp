//
//  BulletNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BulletNode.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "Bullet.h"
#include "BulletType.h"
#include "DataManager.h"
#include "BRConfigData.h"

using namespace std;

BulletNode* BulletNode::create(const Bullet* bullet, float duration)
{
    BulletNode *ret = new (nothrow) BulletNode();
    if (ret && ret->init(bullet, duration))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BulletNode::BulletNode()
:_observer(nullptr)
,_bullet(nullptr)
,_isPVR(false)
,_speedScheduler(nullptr)
,_actionManager(nullptr)
,_angel(-1.0f)
{
    
}

BulletNode::~BulletNode()
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

bool BulletNode::init(const Bullet* bullet, float duration)
{
    if (Node::init()) {
        _bullet = bullet;
        
        auto type = bullet->getBulletType();
        if (type) {
            const auto& name = type->getRenderKey();
            auto data = DataManager::getInstance()->getBRConfigData(name);
            bool enabled(false);
            if (data) {
                const auto& file = data->getResource();
                if (file.length() > 0) {
                    static string file("jian-all/jian");
                    addActionNode(file, false, 0, -1, duration);
                    _isPVR = true;
                    
                    enabled = true;
                } else if (name.find(SPELL_NAME_FIREBALL) != string::npos) {
                    auto yan = addActionNode("particle/yan.plist", false, 0, -1, -1);
                    yan->setPosition(-40, 0);
                    addActionNode("particle/huo.plist", false, 0, -1, -1);
                    addActionNode("huoqiu.csb", true, 0, -1, duration);
                    
                    enabled = true;
                }
            }
            
            if (!enabled) {
                onBulletDisabled();
            }
        }
        
        update(true);
        
        return true;
    }
    
    return false;
}

#pragma mark - public
void BulletNode::registerObserver(BulletNodeObserver *observer)
{
    _observer = observer;
}

const Bullet* BulletNode::getBullet() const
{
    return _bullet;
}

void BulletNode::update()
{
    update(false);
}

#pragma mark - protected
void BulletNode::update(bool newCreated)
{
    if (_bullet) {
        const auto& currentPos = _bullet->getPos();
        const auto& targetPos = _bullet->targetPos();
        
        if (newCreated && (_isPVR == (currentPos.x < targetPos.x))) {
            setScaleX(-1 * getScaleX());
        }
        
        const float deltaX(currentPos.x - targetPos.x);
        const float deltaY(currentPos.y - targetPos.y);
        
        if (abs(deltaX) > 0) {
            if (!_isPVR) {
                _angel = (-180.0f) * atanf(deltaY / deltaX) / M_PI;
                setRotation(_angel);
            }
        } else if (newCreated && _observer) {
            _observer->onBulletNodeReachedTarget(this);
        }
        
#if false
        CCLOG("========= %d - %d - %d - %d - %.1f", currentPos.x, currentPos.y, targetPos.x, targetPos.y, _angel);
#endif
        
        if (_bullet->isExploded()) {
            if (_observer) {
                _observer->onBulletNodeExploded(this);
            }
        }
    } else {
        assert(false);
    }
}

Node* BulletNode::addActionNode(const string& file, bool loop, int startIdx, int endIdx, float duration)
{
    if (file.size() > 0 && startIdx >= 0) {
        if (file.find(".csb") != string::npos) {
            auto node = CSLoader::createNode(file);
            addChild(node);
            
            auto animation = CSLoader::createTimeline(file);
            node->runAction(animation);
            if (endIdx > 0) {
                animation->gotoFrameAndPlay(startIdx, endIdx, loop);
            } else {
                animation->gotoFrameAndPlay(startIdx, loop);
            }
            animation->setLastFrameCallFunc(nullptr);
            
            // update speed
            if (_speedScheduler && duration > 0) {
                auto time = (float)animation->getDuration() / 60.0f;
                _speedScheduler->setTimeScale(time / duration);
            }
            
            return node;
        } else if (file.find(".plist") != string::npos) {
            auto node = ParticleSystemQuad::create(file);
            addChild(node);
            
            node->setAutoRemoveOnFinish(!loop);
        } else {
            auto frame = CocosUtils::getFrame(file, startIdx);
            auto node = Sprite::createWithSpriteFrame(frame);
            addChild(node);
            
            setScheduler(node);
            
            static const float frameDelay(DEFAULT_FRAME_DELAY);
            auto framesCount = CocosUtils::playAnimation(node, file, loop, startIdx, endIdx, DEFAULT_FRAME_DELAY, nullptr);
            
            // update speed
            if (_speedScheduler && duration > 0) {
                auto time = framesCount * (frameDelay + Director::getInstance()->getAnimationInterval());
                _speedScheduler->setTimeScale(time / duration);
            }
            
            return node;
        }
    }
    
    return nullptr;
}

void BulletNode::onBulletDisabled()
{
    addActionNode("effect-jian.csb", false, 0, -1, -1);
}

void BulletNode::setScheduler(Node* node)
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
        node->setActionManager(_actionManager);
    }
}
