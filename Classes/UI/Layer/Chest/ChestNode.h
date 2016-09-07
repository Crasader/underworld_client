//
//  ChestNode.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ChestNode_h
#define ChestNode_h

#include "cocos2d.h"

USING_NS_CC;

class ChestData;

class ChestNodeObserver
{
public:
    virtual ~ChestNodeObserver() {}
};

class ChestNode: public Node
{
public:
    static ChestNode* create();
    virtual ~ChestNode();
    void registerObserver(ChestNodeObserver *observer);
    void update(const ChestData* data);
    
protected:
    ChestNode();
    virtual bool init() override;
    
private:
    ChestNodeObserver* _observer;
    const ChestData* _data;
};

#endif /* ChestNode_h */
