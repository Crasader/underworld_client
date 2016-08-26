//
//  RuneNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneNode_h
#define RuneNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class RuneData;
class RuneNode;

class RuneNodeObserver
{
public:
    virtual ~RuneNodeObserver() {}
    virtual void onRuneNodeClicked(RuneNode* pSender) = 0;
};

class RuneNode : public ui::Widget
{
public:
    static RuneNode* create(const RuneData* data);
    virtual ~RuneNode();
    void registerObserver(RuneNodeObserver *observer);
    
    void update(const RuneData* data);
    const RuneData* getData() const;
    
private:
    RuneNode();
    bool init(const RuneData* data);
    
private:
    RuneNodeObserver* _observer;
    Sprite* _icon;
    Label* _level;
    Label* _amount;
    const RuneData* _data;
    bool _touchInvalid;
};

#endif /* RuneNode_h */
