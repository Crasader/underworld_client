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
#include "CocosGlobal.h"

USING_NS_CC;

class NoticeData;

class NoticeNodeObserver
{
public:
    virtual ~NoticeNodeObserver() {}
};

class NoticeNode: public Node
{
public:
    static NoticeNode* create(const NoticeData* data);
    virtual ~NoticeNode();
    void registerObserver(NoticeNodeObserver *observer);
    void update(const NoticeData* data);
    
protected:
    NoticeNode();
    bool init(const NoticeData* data);
    Node* createResourceNode(::ResourceType type, int count);
    void adjust();
    
private:
    NoticeNodeObserver* _observer;
    ui::Scale9Sprite* _bg;
    Label* _user;
    Label* _content;
    Label* _time;
    Sprite* _resourceBg;
    std::map<::ResourceType, Node*> _resourceNodes;
};

#endif /* NoticeNode_h */
