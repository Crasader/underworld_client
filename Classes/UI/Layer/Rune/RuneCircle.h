//
//  RuneCircle.h
//  Underworld_Client
//
//  Created by Andy on 16/8/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef RuneCircle_h
#define RuneCircle_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "RuneNode.h"

USING_NS_CC;

class RuneData;

class RuneCircleObserver
{
public:
    virtual ~RuneCircleObserver() {}
    virtual void onRuneCircleClicked(RuneNode* node) = 0;
};

class RuneCircle
: public Node
, public RuneNodeObserver
{
public:
    static RuneCircle* create();
    virtual ~RuneCircle();
    void registerObserver(RuneCircleObserver *observer);
    
    void setData(const std::vector<const RuneData*>& datas);
    
private:
    RuneCircle();
    virtual bool init() override;
    
    // RuneNodeObserver
    virtual void onRuneNodeClicked(RuneNode* pSender) override;
    
private:
    RuneCircleObserver* _observer;
    Node* _circle;
    std::vector<RuneNode*> _nodes;
};

#endif /* RuneCircle_h */
