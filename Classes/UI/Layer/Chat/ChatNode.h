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

class ChatData;

class ChatNodeObserver
{
public:
    virtual ~ChatNodeObserver() {}
};

class ChatNode: public Node
{
public:
    static ChatNode* create(const ChatData* data);
    virtual ~ChatNode();
    void registerObserver(ChatNodeObserver *observer);
    void update(const ChatData* data);
    
protected:
    ChatNode();
    bool init(const ChatData* data);
    void adjust();
    
private:
    ChatNodeObserver* _observer;
    const ChatData* _data;
    Node* _icon;
    ui::Scale9Sprite* _bg;
    Label* _user;
    Label* _content;
    Label* _time;
};

#endif /* ChatNode_h */
