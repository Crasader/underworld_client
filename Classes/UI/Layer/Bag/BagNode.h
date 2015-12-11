//
//  BagNode.h
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BagNode_h
#define BagNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class BagNodeObserver
{
public:
    virtual ~BagNodeObserver() {}
};

class BagNode: public Node
{
public:
    static BagNode* create();
    virtual ~BagNode();
    void registerObserver(BagNodeObserver *observer);
    
protected:
    BagNode();
    bool init();
    
private:
    BagNodeObserver* _observer;
};

#endif /* BagNode_h */
