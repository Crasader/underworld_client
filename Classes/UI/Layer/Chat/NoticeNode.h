//
//  NoticeNode.h
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef NoticeNode_h
#define NoticeNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class NoticeNodeObserver
{
public:
    virtual ~NoticeNodeObserver() {}
};

class NoticeNode: public Node
{
public:
    static NoticeNode* create();
    virtual ~NoticeNode();
    void registerObserver(NoticeNodeObserver *observer);
    
protected:
    NoticeNode();
    bool init();
    
private:
    NoticeNodeObserver* _observer;
};

#endif /* NoticeNode_h */
