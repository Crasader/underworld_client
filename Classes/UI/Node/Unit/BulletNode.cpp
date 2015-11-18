//
//  BulletNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BulletNode.h"
#include "cocostudio/CocoStudio.h"
#include "Bullet.h"
#include "BulletType.h"

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
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}

BulletNode::BulletNode()
:_observer(nullptr)
,_actionNode(nullptr)
,_bullet(nullptr)
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
        
#if true
        _actionNode = Sprite::create("GameImages/test/jian.png");
        addChild(_actionNode);
#else
        const BulletType* type = bullet->getBulletType();
        string csbFile = "hongzidan.csb";
        _actionNode = CSLoader::createNode(csbFile);
        addChild(_actionNode);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        _actionNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
#endif
        
        update(true);
        
        return true;
    }
    
    return false;
}

void BulletNode::update(bool newCreated)
{
    if (_bullet) {
        const Coordinate& currentPos = _bullet->getPos();
        const Coordinate& targetPos = _bullet->targetPos();
        
        if (currentPos.x > targetPos.x) {
            setScaleX(-1 * getScaleX());
        }
        
        int deltaX = abs(currentPos.x - targetPos.x);
        int deltaY = abs(currentPos.y - targetPos.y);
        
        if (deltaX > 0) {
            float angel = 180.0f * atanf(deltaY / deltaX) / M_PI * getScaleX();
            setRotation(angel);
        } else {
            // so close to the target
            if (newCreated) {
                if (_observer) {
                    _observer->onBulletNodeReachedTarget(this);
                }
            }
        }
    } else {
        setScaleX(-1);
        float angel = 180.0f * atanf(1) / M_PI;
        setRotation(angel * getScaleX());
    }
}