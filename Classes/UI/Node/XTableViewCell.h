//
//  XTableViewCell.h
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef XTableViewCell_h
#define XTableViewCell_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class XTableViewCell: public TableViewCell
{
public:
    CREATE_FUNC(XTableViewCell);
    Node* getNode(int idx) const;
    void setNode(Node* node, int idx);
    void resetNode(int idx);
    
private:
    std::unordered_map<int, Node*> _nodes;
};

#endif /* XTableViewCell_h */
