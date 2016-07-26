//
//  NoticeNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "NoticeNode.h"

using namespace std;

NoticeNode* NoticeNode::create()
{
    NoticeNode *ret = new (nothrow) NoticeNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

NoticeNode::NoticeNode()
:_observer(nullptr)
{
    
}

NoticeNode::~NoticeNode()
{
    removeAllChildren();
}

void NoticeNode::registerObserver(NoticeNodeObserver *observer)
{
    _observer = observer;
}

bool NoticeNode::init()
{
    if (Node::init()) {
        
        return true;
    }
    
    return false;
}
