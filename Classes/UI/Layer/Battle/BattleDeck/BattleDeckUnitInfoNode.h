//
//  BattleDeckBattleDeckUnitInfoNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckBattleDeckUnitInfoNode_h
#define BattleDeckBattleDeckUnitInfoNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class UnitBase;
} }

class BattleDeckUnitInfoNodeObserver
{
public:
    virtual ~BattleDeckUnitInfoNodeObserver() {}
};

class BattleDeckUnitInfoNode : public Node
{
public:
    static BattleDeckUnitInfoNode* create(const UnderWorld::Core::UnitBase* unit);
    virtual ~BattleDeckUnitInfoNode();
    
    void registerObserver(BattleDeckUnitInfoNodeObserver *observer);
    void update(const UnderWorld::Core::UnitBase* unit);
    void update(const std::string& name);
    
protected:
    BattleDeckUnitInfoNode();
    virtual bool init() override;
    
private:
    BattleDeckUnitInfoNodeObserver *_observer;
    const UnderWorld::Core::UnitBase* _unit;
    // top
    Sprite* _unitIcon;
    Sprite* _qualityIcon;
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

#endif /* BattleDeckBattleDeckUnitInfoNode_h */
