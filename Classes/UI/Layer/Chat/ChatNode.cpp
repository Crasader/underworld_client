//
//  ChatNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatNode.h"
#include "ChatData.h"

using namespace std;

ChatNode* ChatNode::create(const ChatData* data)
{
    auto ret = new (nothrow) ChatNode();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChatNode::ChatNode()
:_observer(nullptr)
,_data(nullptr)
,_icon(nullptr)
,_bg(nullptr)
,_user(nullptr)
,_content(nullptr)
,_time(nullptr) {}

ChatNode::~ChatNode()
{
    removeAllChildren();
}

void ChatNode::registerObserver(ChatNodeObserver *observer)
{
    _observer = observer;
}

bool ChatNode::init(const ChatData* data)
{
    if (Node::init()) {
        
        return true;
    }
    
    return false;
}

void ChatNode::update(const ChatData* data)
{
    
}

void ChatNode::adjust()
{
    
}
