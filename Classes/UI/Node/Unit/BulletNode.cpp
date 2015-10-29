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
,_bullet(nullptr)
{
    
}

BulletNode::~BulletNode()
{
    removeAllChildren();
}

void BulletNode::registerObserver(BulletNodeObserver *observer)
{
    _observer = observer;
}

bool BulletNode::init(const Bullet* bullet)
{
    if (Node::init())
    {
        _bullet = bullet;
        
        const BulletType* type = bullet->getBulletType();
        
        string csbFile = "";
        Node *mainNode = CSLoader::createNode(csbFile);
        addChild(mainNode);
        cocostudio::timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        
        return true;
    }
    
    return false;
}
