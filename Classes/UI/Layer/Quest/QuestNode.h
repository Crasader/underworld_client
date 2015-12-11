//
//  QuestNode.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestNode_h
#define QuestNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class QuestNodeObserver
{
public:
    virtual ~QuestNodeObserver() {}
};

class QuestNode: public Node
{
public:
    static QuestNode* create();
    virtual ~QuestNode();
    void registerObserver(QuestNodeObserver *observer);
    
protected:
    QuestNode();
    bool init();
    
private:
    QuestNodeObserver* _observer;
};

#endif /* QuestNode_h */
