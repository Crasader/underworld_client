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

class ChatData;
class ObjectBriefNode;
class XButton;

class NoticeNodeObserver
{
public:
    virtual ~NoticeNodeObserver() {}
    virtual void onNoticeNodeGet(const ChatData* data) = 0;
    virtual void onNoticeNodeDelete(const ChatData* data) = 0;
};

class NoticeNode: public Node
{
public:
    static NoticeNode* create(ChatType type, float width, const ChatData* data);
    virtual ~NoticeNode();
    void registerObserver(NoticeNodeObserver *observer);
    void update(const ChatData* data);
    
protected:
    NoticeNode();
    bool init(ChatType type, float width, const ChatData* data);
    void adjust();
    
private:
    NoticeNodeObserver* _observer;
    ChatType _type;
    float _width;
    const ChatData* _data;
    ui::Scale9Sprite* _bg;
    Label* _user;
    Label* _content;
    Label* _time;
    Sprite* _resourceBg;
    XButton* _button;
    bool _touchInvalid;
    std::vector<ObjectBriefNode*> _resourceNodes;
};

#endif /* NoticeNode_h */
