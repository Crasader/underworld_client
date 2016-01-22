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
    class Camp;
    class UnitBase;
} }

class UnitSimpleInfoNodeObserver
{
public:
    virtual ~UnitSimpleInfoNodeObserver() {}
    virtual void onUnitSimpleInfoNodeTouched(Ref* pSender, ui::Widget::TouchEventType type, const UnderWorld::Core::UnitBase* unit) = 0;
};

class UnitSimpleInfoNode : public Node
{
public:
    static UnitSimpleInfoNode* create(const AbstractData* data);
    static UnitSimpleInfoNode* create(const UnderWorld::Core::Camp* camp, const UnderWorld::Core::UnitBase* unit);
    virtual ~UnitSimpleInfoNode();
    
    void registerObserver(UnitSimpleInfoNodeObserver *observer);
    void update(const AbstractData* data);
    void update(const UnderWorld::Core::Camp* camp, const UnderWorld::Core::UnitBase* unit);
    
protected:
    UnitSimpleInfoNode();
    virtual bool init() override;
    
private:
    UnitSimpleInfoNodeObserver *_observer;
    const AbstractData* _data;
    const UnderWorld::Core::Camp* _camp;
    const UnderWorld::Core::UnitBase* _unit;
    // top
    Sprite* _icon;
    Label* _countLabel;
    Label* _levelLabel;
};

#endif /* UnitSimpleInfoNode_h */
