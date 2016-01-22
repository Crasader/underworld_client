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

class AbstractData;

namespace UnderWorld { namespace Core {
    class UnitBase;
} }

class UnitInfoNodeObserver
{
public:
    virtual ~UnitInfoNodeObserver() {}
};

class UnitInfoNode : public Node
{
public:
    static UnitInfoNode* create(const AbstractData* data);
    static UnitInfoNode* create(const UnderWorld::Core::UnitBase* unit);
    static UnitInfoNode* create(std::nullptr_t nullp);
    virtual ~UnitInfoNode();
    
    void registerObserver(UnitInfoNodeObserver *observer);
    void update(const AbstractData* data);
    void update(const UnderWorld::Core::UnitBase* unit);
    void update(std::nullptr_t nullp);
    
protected:
    UnitInfoNode();
    virtual bool init() override;
    
private:
    UnitInfoNodeObserver *_observer;
    const AbstractData* _data;
    const UnderWorld::Core::UnitBase* _unit;
    // top
    Sprite* _raceIcon;
    Label* _nameLabel;
    Label* _hpLabel;
    Label* _armorLabel;
    Label* _armorPreferLabel;
    Label* _dmgLabel;
    Label* _atkSpeedLabel;
    Label* _atkRangeLabel;
    // bottom
    Sprite* _skillIcon;
    Label* _skillLevelLabel;
    Label* _skillNameLabel;
    Label* _skillDescriptionLabel;
};

#endif /* UnitInfoNode_h */
