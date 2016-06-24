//
//  CardInfoNode.h
//  Underworld_Client
//
//  Created by Andy on 16/6/24.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardInfoNode_h
#define CardInfoNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class UnitBase;
} }

class CardInfoNodeObserver
{
public:
    virtual ~CardInfoNodeObserver() {}
};

class CardInfoNode : public Node
{
public:
    static CardInfoNode* create();
    virtual ~CardInfoNode();
    
    void registerObserver(CardInfoNodeObserver *observer);
    void update(const std::string& name);
    
protected:
    CardInfoNode();
    virtual bool init() override;
    
private:
    CardInfoNodeObserver *_observer;
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

#endif /* CardInfoNode_h */
