//
//  QuestCell.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestCell_h
#define QuestCell_h

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class QuestNode;

class QuestCell: public TableViewCell
{
public:
    CREATE_FUNC(QuestCell);
    QuestNode* getNode() const;
    void setNode(QuestNode* node);
    void resetNode();
    
private:
    QuestNode* _node;
};

#endif /* QuestCell_h */
