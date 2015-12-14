//
//  ChatNode.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatNode_h
#define ChatNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class ChatNodeObserver
{
public:
    virtual ~ChatNodeObserver() {}
};

class ChatNode: public Node
{
public:
    static ChatNode* create();
    virtual ~ChatNode();
    void registerObserver(ChatNodeObserver *observer);
    
protected:
    ChatNode();
    bool init();
    
private:
    ChatNodeObserver* _observer;
};

#endif /* ChatNode_h */
