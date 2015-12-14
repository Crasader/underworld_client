//
//  ChatCell.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatCell_h
#define ChatCell_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class ChatNode;

class ChatCell: public TableViewCell
{
public:
    CREATE_FUNC(ChatCell);
    ChatNode* getNode() const;
    void setNode(ChatNode* node);
    void resetNode();
    
private:
    ChatNode* _node;
};

#endif /* ChatCell_h */
