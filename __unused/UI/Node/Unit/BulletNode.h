//
//  BulletNode.h
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BulletNode_h
#define BulletNode_h

#include "cocos2d.h"

USING_NS_CC;

// UnderWorld::Core
namespace UnderWorld { namespace Core { class Bullet; } }

using namespace UnderWorld::Core;

// Observer
class BulletNode;
class BulletNodeObserver
{
public:
    virtual ~BulletNodeObserver() {}
    virtual void onBulletNodeReachedTarget(BulletNode* node) = 0;
    virtual void onBulletNodeExploded(BulletNode* node) = 0;
};

class BulletNode : public Node
{
public:
    static BulletNode* create(const Bullet* bullet, float duration);
    virtual ~BulletNode();
    
    void registerObserver(BulletNodeObserver *observer);
    const Bullet* getBullet() const;
    void update();
    
protected:
    BulletNode();
    
    bool init(const Bullet* bullet, float duration);
    void update(bool newCreated);
    Node* addActionNode(const std::string& file, bool loop, int startIdx, int endIdx, float duration);
    void onBulletDisabled();
    
    void setScheduler(Node* node);
    
protected:
    BulletNodeObserver *_observer;
    const Bullet* _bullet;
    bool _isPVR;
    Scheduler* _speedScheduler;
    ActionManager* _actionManager;
    float _angel;
};

#endif /* BulletNode_h */
