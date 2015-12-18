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

class ObjectData;

class BagNodeObserver
{
public:
    virtual ~BagNodeObserver() {}
};

class BagNode: public Node
{
public:
    static BagNode* create(const ObjectData* data, ssize_t idx, int column);
    virtual ~BagNode();
    void registerObserver(BagNodeObserver *observer);
    
    ssize_t getIdx() const;
    int getColumn() const;
    void update(const ObjectData* data, ssize_t idx, int column);
    
protected:
    BagNode();
    bool init(const ObjectData* data, ssize_t idx, int column);
    
private:
    BagNodeObserver* _observer;
    const ObjectData* _data;
    ssize_t _idx;
    int _column;
    Sprite* _icon;
    Label* _nameLabel;
    Label* _descriptionLabel;
};

#endif /* BagNode_h */
