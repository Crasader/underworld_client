//
//  ChatCell.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatCell.h"

ChatNode* ChatCell::getNode() const
{
    return _node;
}

void ChatCell::setNode(ChatNode* node)
{
    assert(!_node);
    _node = node;
}

void ChatCell::resetNode()
{
    _node = nullptr;
}
