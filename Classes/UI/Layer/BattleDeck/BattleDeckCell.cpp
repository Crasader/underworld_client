//
//  BattleDeckCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleDeckCell.h"

Node* BattleDeckCell::getNode(int idx) const
{
    if (_nodes.find(idx) != _nodes.end()) {
        return _nodes.at(idx);
    }
    
    return nullptr;
}

void BattleDeckCell::setNode(Node* node, int idx)
{
    if (node) {
        if (_nodes.find(idx) != _nodes.end()) {
            _nodes.at(idx) = node;
        } else {
            _nodes.insert(std::make_pair(idx, node));
        }
    }
}

void BattleDeckCell::resetNode(int idx)
{
    if (_nodes.find(idx) != _nodes.end()) {
        _nodes.erase(idx);
    }
}
