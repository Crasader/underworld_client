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
        
#if true
        const Unit* trigger = bullet->getTrigger();
        if (trigger) {
            const string& name = trigger->getUnitType()->getName();
            string file;
            if (name == WOLF_WIZARD) {
                file = "effect-fireball.csb";
            } else if (name == VAMPIRE_WIZARD) {
                file = "effect-fireball-1.csb";
            } else if (name == VAMPIRE_TOWER) {
                file = "Vampire-tower-attack.csb";
            } else if (name == WOLF_CORE) {
                file = "wolf-base-attack-1.csb";
            } else if (name == VAMPIRE_CORE) {
                file = "Vampire-base-attack-1.csb";
            } else {
                file = "effect-jian.csb";
            }
            
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