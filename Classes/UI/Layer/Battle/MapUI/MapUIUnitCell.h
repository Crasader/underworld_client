//
//  MapUIUnitCell.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUIUnitCell_h
#define MapUIUnitCell_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class MapUIUnitNode;

class MapUIUnitCell: public TableViewCell
{
public:
    CREATE_FUNC(MapUIUnitCell);
    MapUIUnitNode* getUnitNode() const;
    void setUnitNode(MapUIUnitNode* node);
    void resetUnitNode();
    
private:
    MapUIUnitNode* _node;
};

#endif /* MapUIUnitCell_h */
