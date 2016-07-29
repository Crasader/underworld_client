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

class ChatData;
class ObjectBriefNode;

class NoticeNodeObserver
{
public:
    virtual ~NoticeNodeObserver() {}
};

class NoticeNode: public Node
{
public:
    static NoticeNode* create(float width, const ChatData* data);
    virtual ~NoticeNode();
    void registerObserver(NoticeNodeObserver *observer);
    void update(const ChatData* data);
    
protected:
    NoticeNode();
    bool init(float width, const ChatData* data);
    void adjust();
    
private:
    NoticeNodeObserver* _observer;
    float _width;
    ui::Scale9Sprite* _bg;
    Label* _user;
    Label* _content;
    Label* _time;
    Sprite* _resourceBg;
    std::vector<ObjectBriefNode*> _resourceNodes;
};

#endif /* NoticeNode_h */
