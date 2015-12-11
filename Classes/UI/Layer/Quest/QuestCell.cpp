//
//  QuestCell.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestCell.h"

QuestNode* QuestCell::getNode() const
{
    return _node;
}

void QuestCell::setNode(QuestNode* node)
{
    assert(!_node);
    _node = node;
}

void QuestCell::resetNode()
{
    _node = nullptr;
}