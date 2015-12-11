//
//  BagCell.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BagCell.h"

BagNode* BagCell::getNode() const
{
    return _node;
}

void BagCell::setNode(BagNode* node)
{
    assert(!_node);
    _node = node;
}

void BagCell::resetNode()
{
    _node = nullptr;
}
