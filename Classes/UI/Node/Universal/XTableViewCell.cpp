//
//  XTableViewCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "XTableViewCell.h"

Node* XTableViewCell::getNode(int idx) const
{
    auto iter(_nodes.find(idx));
    if (iter != _nodes.end()) {
        return iter->second;
    }
    
    return nullptr;
}

void XTableViewCell::setNode(Node* node, int idx)
{
    if (node) {
        auto iter(_nodes.find(idx));
        if (iter != _nodes.end()) {
            iter->second = node;
        } else {
            _nodes.insert(std::make_pair(idx, node));
        }
    }
}

void XTableViewCell::resetNode(int idx)
{
    _nodes.erase(idx);
}
