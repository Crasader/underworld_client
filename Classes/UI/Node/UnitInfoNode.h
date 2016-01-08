//
//  UnitInfoNode.h
//  Underworld_Client
//
//  Created by Andy on 16/1/8.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UnitInfoNode_h
#define UnitInfoNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class SoldierData;

class UnitInfoNodeObserver
{
public:
    virtual ~UnitInfoNodeObserver() {}
};

class UnitInfoNode : public Node
{
public:
    static UnitInfoNode* create(const SoldierData* data);
    virtual ~UnitInfoNode();
    
    void registerObserver(UnitInfoNodeObserver *observer);
    void update(const SoldierData* data);
    
protected:
    UnitInfoNode();
    bool init(const SoldierData* data);
    
private:
    UnitInfoNodeObserver *_observer;
    const SoldierData* _data;
};

#endif /* UnitInfoNode_h */
