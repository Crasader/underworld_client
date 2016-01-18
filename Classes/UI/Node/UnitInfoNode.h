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
    class Unit;
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
    static UnitInfoNode* create(const UnderWorld::Core::Unit* unit);
    static UnitInfoNode* create(std::nullptr_t nullp);
    virtual ~UnitInfoNode();
    
    void registerObserver(UnitInfoNodeObserver *observer);
    void update(const AbstractData* data);
    void update(const UnderWorld::Core::Unit* unit);
    void update(std::nullptr_t nullp);
    
protected:
    UnitInfoNode();
    virtual bool init() override;
    
private:
    UnitInfoNodeObserver *_observer;
    const AbstractData* _data;
    const UnderWorld::Core::Unit* _unit;
    // top
    Sprite* _raceIcon;
    Label* _nameLabel;
    LabelAtlas* _hpLabel;
    LabelAtlas* _armorLabel;
    LabelAtlas* _dmgRateLabel;
    LabelAtlas* _dmgLabel;
    LabelAtlas* _dpsLabel;
    LabelAtlas* _rangeLabel;
    // bottom
    Sprite* _skillIcon;
    LabelAtlas* _skillLevelLabel;
    Label* _skillNameLabel;
    Label* _skillDescriptionLabel;
};

#endif /* UnitInfoNode_h */
