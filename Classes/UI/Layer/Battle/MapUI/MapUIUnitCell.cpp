//
//  MapUIUnitCell.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUIUnitCell.h"

MapUIUnitNode* MapUIUnitCell::getUnitNode() const
{
    return _node;
}

void MapUIUnitCell::setUnitNode(MapUIUnitNode* node)
{
    assert(!_node);
    _node = node;
}

void MapUIUnitCell::resetUnitNode()
{
    _node = nullptr;
}
