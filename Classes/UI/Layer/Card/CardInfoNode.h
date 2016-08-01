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

class CardNode;

namespace UnderWorld { namespace Core {
    class UnitBase;
} }

class CardInfoNode : public Node
{
public:
    typedef std::function<void(const std::string&, int)> Callback;
    
public:
    static CardInfoNode* create(const Callback& callback);
    virtual ~CardInfoNode();
    
    void update(const std::string& name);
    
protected:
    CardInfoNode();
    bool init(const Callback& callback);
    void updateCost(int count);
    void addLabel(Label** label);
    
private:
    std::string _name;
    int _cost;
    Callback _callback;
    const UnderWorld::Core::UnitBase* _unit;
    
    // ui
    CardNode* _cardNode;
    Label* _level;
    Label* _quality;
    Label* _population;
    Label* _description;
    Label* _hp;
    Label* _armor;
    Label* _atkType;
    Label* _dmg;
    Label* _atkSpeed;
    Label* _atkRange;
    ui::Button* _button;
};

#endif /* CardInfoNode_h */
