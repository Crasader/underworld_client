//
//  FormationCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FormationCell.h"

Node* FormationCell::getNode(int idx) const
{
    if (_nodes.find(idx) != _nodes.end()) {
        return _nodes.at(idx);
    }
    
    return nullptr;
}

void FormationCell::setNode(Node* node, int idx)
{
    if (node) {
        if (_nodes.find(idx) != _nodes.end()) {
            _nodes.at(idx) = node;
        } else {
            _nodes.insert(std::make_pair(idx, node));
        }
    }
}

void FormationCell::resetNode(int idx)
{
    if (_nodes.find(idx) != _nodes.end()) {
        _nodes.erase(idx);
    }
}
