//
//  ChestNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ChestNode.h"
#include "CocosUtils.h"
#include "ChestData.h"

using namespace std;

ChestNode* ChestNode::create()
{
    auto ret = new (nothrow) ChestNode();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ChestNode::ChestNode()
:_observer(nullptr)
,_data(nullptr) {}

ChestNode::~ChestNode()
{
    removeAllChildren();
}

void ChestNode::registerObserver(ChestNodeObserver *observer)
{
    _observer = observer;
}

bool ChestNode::init()
{
    if (Node::init()) {
        
        return true;
    }
    
    return false;
}

void ChestNode::update(const ChestData* data)
{
    if (_data != data) {
        _data = data;
    }
}
