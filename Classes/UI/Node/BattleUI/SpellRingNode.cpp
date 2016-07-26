//
//  SpellRingNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SpellRingNode.h"
#include "CocosUtils.h"

using namespace std;

SpellRingNode* SpellRingNode::create(const string& file, float duration, const Callback& callback)
{
    SpellRingNode *p = new (nothrow) SpellRingNode();
    if(p && p->init(file, duration, callback)) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

SpellRingNode::SpellRingNode()
:_callback(nullptr) {}

SpellRingNode::~SpellRingNode()
{
    removeFromParent();
}

bool SpellRingNode::init(const string& file, float duration, const Callback& callback)
{
    if(Node::init())
    {
        if (!file.empty()) {
            auto ring = CocosUtils::playAnimation(file, 0, true);
            addChild(ring);
            
            if (duration > 0) {
                _callback = callback;
                _scheduler->schedule(CC_SCHEDULE_SELECTOR(SpellRingNode::onExpire), this, duration, false);
            }
        }
        
        return true;
    }
    
    return false;
}

void SpellRingNode::onExpire(float dt)
{
    _scheduler->unschedule(CC_SCHEDULE_SELECTOR(SpellRingNode::onExpire), this);
    if (_callback) {
        _callback(this);
    }
}
