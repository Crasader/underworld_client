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
#include "Bullet.h"
#include "BulletType.h"
#include "DataManager.h"
#include "BRConfigData.h"

using namespace std;
using namespace UnderWorld::Core;

BulletNode* BulletNode::create(const Bullet* Bullet)
{
    BulletNode *ret = new (nothrow) BulletNode();
    if (ret && ret->init(Bullet))
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
,_angel(-1.0f)
{
    
}

BulletNode::~BulletNode()
{
    removeAllChildren();
}

const Bullet* BulletNode::getBullet() const
{
    return _bullet;
}

void BulletNode::registerObserver(BulletNodeObserver *observer)
{
    _observer = observer;
}

void BulletNode::update()
{
    update(false);
}

bool BulletNode::init(const Bullet* bullet)
{
    if (Node::init())
    {
        _bullet = bullet;
        
        const BulletType* type = bullet->getBulletType();
        if (type) {
            const string& name = type->getRenderKey();
            const BRConfigData* data = DataManager::getInstance()->getBRConfigData(name);
            if (data) {
                const string& file = data->getResource();
                if (file.length() > 0) {
                    addActionNode(file, false);
                } else {
                    if (name.find(SPELL_NAME_FIREBALL) != string::npos) {
                        Node* yan = addParticle("particle/yan.plist");
                        yan->setPosition(-40, 0);
                        addParticle("particle/huo.plist");
                        addActionNode("huoqiu.csb", true);
                    } else {
                        onBulletDisabled();
                    }
                }
            } else {
                onBulletDisabled();
            }
        } else {
            onBulletDisabled();
        }
        
        update(true);
        
        return true;
    }
    
    return false;
}

void BulletNode::update(bool newCreated)
{
    if (_bullet) {
        const Coordinate32& currentPos = _bullet->getPos();
        const Coordinate32& targetPos = _bullet->targetPos();
        
        if (newCreated && currentPos.x > targetPos.x) {
            setScaleX(-1 * getScaleX());
        }
        
        const float deltaX(currentPos.x - targetPos.x);
        const float deltaY(currentPos.y - targetPos.y);
        
        if (abs(deltaX) > 0) {
            _angel = (-180.0f) * atanf(deltaY / deltaX) / M_PI;
            setRotation(_angel);
        } else {
            // so close to the target when created
            if (newCreated) {
                if (_observer) {
                    _observer->onBulletNodeReachedTarget(this);
                }
            }
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

Node* BulletNode::addActionNode(const string& file, bool loop)
{
    const string path = file + ".csb";
    Node* node = CSLoader::createNode(path);
    addChild(node);
    cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(path);
    node->runAction(action);
    action->gotoFrameAndPlay(0, loop);
    return node;
}

Node* BulletNode::addParticle(const string& file)
{
    ParticleSystemQuad *node = ParticleSystemQuad::create(file);
    addChild(node);
    return node;
}

void BulletNode::onBulletDisabled()
{
    addActionNode("effect-jian", false);
}
