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
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core { class Bullet; } }

class BulletNodeObserver
{
public:
    virtual ~BulletNodeObserver() {}
};

class BulletNode : public Node
{
public:
    static BulletNode* create(const UnderWorld::Core::Bullet* bullet);
    virtual ~BulletNode();
    void registerObserver(BulletNodeObserver *observer);
    void update();
    
protected:
    BulletNode();
    bool init(const UnderWorld::Core::Bullet* bullet);
    
private:
    BulletNodeObserver *_observer;
    const UnderWorld::Core::Bullet* _bullet;
};

#endif /* BulletNode_h */
