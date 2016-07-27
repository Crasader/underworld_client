//
//  UserOperationNode.h
//  Underworld_Client
//
//  Created by Andy on 16/7/27.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UserOperationNode_h
#define UserOperationNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UniversalButton;

class UserOperationNodeObserver
{
public:
    virtual ~UserOperationNodeObserver() {}
};

class UserOperationNode: public Node
{
public:
    static UserOperationNode* create(int uid);
    virtual ~UserOperationNode();
    void registerObserver(UserOperationNodeObserver *observer);
    void update(int uid);
    
protected:
    UserOperationNode();
    bool init(int uid);
    void adjust();
    void onButtonClicked(int idx);
    
private:
    UserOperationNodeObserver* _observer;
    ui::Scale9Sprite* _bg;
    int _uid;
    std::vector<UniversalButton*> _buttons;
};

#endif /* UserOperationNode_h */
