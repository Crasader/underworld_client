//
//  BagCell.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BagCell_h
#define BagCell_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BagNode;

class BagCell: public TableViewCell
{
public:
    CREATE_FUNC(BagCell);
    BagNode* getNode(int idx) const;
    void setNode(BagNode* node, int idx);
    void resetNode(int idx);
    
private:
    std::unordered_map<int, BagNode*> _nodes;
};

#endif /* BagCell_h */
