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
class AvatarNode;

class ChatNodeObserver
{
public:
    virtual ~ChatNodeObserver() {}
    virtual void onChatNodeTouchedAvatar(const ChatData* data) = 0;
};

class ChatNode: public Node
{
public:
    static ChatNode* create(float width, const ChatData* data);
    virtual ~ChatNode();
    void registerObserver(ChatNodeObserver *observer);
    void update(const ChatData* data);
    
protected:
    ChatNode();
    bool init(float width, const ChatData* data);
    void createDialog(bool isMe);
    void adjust(bool isMe);
    
private:
    ChatNodeObserver* _observer;
    float _width;
    float _dialogWidth;
    const ChatData* _data;
    AvatarNode* _avatar;
    ui::Scale9Sprite* _dialogBg;
    Label* _user;
    Label* _content;
    Label* _time;
};

#endif /* ChatNode_h */
