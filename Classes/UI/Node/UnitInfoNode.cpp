//
//  UnitInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/8.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UnitInfoNode.h"
#include "SoldierData.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;

UnitInfoNode* UnitInfoNode::create(const SoldierData* data)
{
    UnitInfoNode *ret = new (nothrow) UnitInfoNode();
    if (ret && ret->init(data))
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

UnitInfoNode::UnitInfoNode()
:_observer(nullptr)
,_data(nullptr)
{
    
}

UnitInfoNode::~UnitInfoNode()
{
    removeAllChildren();
}

bool UnitInfoNode::init(const SoldierData* data)
{
    if (Node::init()) {
        
        return true;
    }
    
    return false;
}

void UnitInfoNode::registerObserver(UnitInfoNodeObserver *observer)
{
    _observer = observer;
}

void UnitInfoNode::update(const SoldierData* data)
{
    _data = data;
}
