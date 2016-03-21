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
    class TechTree;
} }

class BattleDeckUnitInfoNodeObserver
{
public:
    virtual ~BattleDeckUnitInfoNodeObserver() {}
};

class BattleDeckUnitInfoNode : public Node
{
public:
    static BattleDeckUnitInfoNode* create();
    virtual ~BattleDeckUnitInfoNode();
    
    void registerObserver(BattleDeckUnitInfoNodeObserver *observer);
    void update(const std::string& name, UnderWorld::Core::TechTree* techTree);
    
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
    Label* _descriptionLabel;
};

#endif /* BattleDeckBattleDeckUnitInfoNode_h */
