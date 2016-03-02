//
//  BattleDeckCell.h
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckCell_h
#define BattleDeckCell_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BattleDeckCell: public TableViewCell
{
public:
    CREATE_FUNC(BattleDeckCell);
    Node* getNode(int idx) const;
    void setNode(Node* node, int idx);
    void resetNode(int idx);
    
private:
    std::map<int, Node*> _nodes;
};

#endif /* BattleDeckCell_h */
