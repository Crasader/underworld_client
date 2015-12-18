//
//  BagCell.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BagCell.h"

BagNode* BagCell::getNode(int idx) const
{
    if (_nodes.find(idx) != _nodes.end()) {
        return _nodes.at(idx);
    }
    
    return nullptr;
}

void BagCell::setNode(BagNode* node, int idx)
{
    if (node) {
        if (_nodes.find(idx) != _nodes.end()) {
            _nodes.at(idx) = node;
        } else {
            _nodes.insert(std::make_pair(idx, node));
        }
    }
}

void BagCell::resetNode(int idx)
{
    if (_nodes.find(idx) != _nodes.end()) {
        _nodes.erase(idx);
    }
}
