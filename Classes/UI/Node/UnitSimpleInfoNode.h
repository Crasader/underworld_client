//
//  UnitSimpleInfoNode.h
//  Underworld_Client
//
//  Created by Andy on 16/1/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UnitSimpleInfoNode_h
#define UnitSimpleInfoNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class AbstractData;

namespace UnderWorld { namespace Core {
    class Unit;
} }

class UnitSimpleInfoNodeObserver
{
public:
    virtual ~UnitSimpleInfoNodeObserver() {}
};

class UnitSimpleInfoNode : public Node
{
public:
    static UnitSimpleInfoNode* create(const AbstractData* data);
    static UnitSimpleInfoNode* create(const UnderWorld::Core::Unit* unit);
    static UnitSimpleInfoNode* create(std::nullptr_t nullp);
    virtual ~UnitSimpleInfoNode();
    
    void registerObserver(UnitSimpleInfoNodeObserver *observer);
    void update(const AbstractData* data);
    void update(const UnderWorld::Core::Unit* unit);
    void update(std::nullptr_t nullp);
    
protected:
    UnitSimpleInfoNode();
    virtual bool init() override;
    
private:
    UnitSimpleInfoNodeObserver *_observer;
    const AbstractData* _data;
    const UnderWorld::Core::Unit* _unit;
    // top
    Sprite* _icon;
    LabelAtlas* _countLabel;
    LabelAtlas* _levelLabel;
};

#endif /* UnitSimpleInfoNode_h */
