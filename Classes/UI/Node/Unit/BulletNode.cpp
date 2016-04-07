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
#include "Unit.h"
#include "UnitType.h"
#include "Bullet.h"
#include "DataManager.h"
#include "URConfigData.h"

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
        
        const Unit* trigger = bullet->getTrigger();
        if (trigger) {
            const string& name = trigger->getUnitBase().getRenderKey();
            const string& file = DataManager::getInstance()->getURConfigData(name)->getBullet();
            if (file.length() > 0) {
                _actionNode = CSLoader::createNode(file);
                addChild(_actionNode);
                cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                _actionNode->runAction(action);
                action->gotoFrameAndPlay(0, false);
            } else {
                _actionNode = Sprite::create("GameImages/test/jian.png");
                addChild(_actionNode);
            }
        } else {
            assert(false);
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
        
//        CCLOG("========= %d - %d - %d - %d - %.1f", currentPos.x, currentPos.y, targetPos.x, targetPos.y, _angel);
        
        if (_bullet->isExploded()) {
            if (_observer) {
                _observer->onBulletNodeExploded(this);
            }
        }
    } else {
        assert(false);
    }
}