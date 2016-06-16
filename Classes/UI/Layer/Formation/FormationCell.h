//
//  FormationCell.h
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FormationCell_h
#define FormationCell_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class FormationCell: public TableViewCell
{
public:
    CREATE_FUNC(FormationCell);
    Node* getNode(int idx) const;
    void setNode(Node* node, int idx);
    void resetNode(int idx);
    
private:
    std::unordered_map<int, Node*> _nodes;
};

#endif /* FormationCell_h */
