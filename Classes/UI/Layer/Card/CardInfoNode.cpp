//
//  CardInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/24.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardInfoNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"
#include "CardNode.h"
#include "Unit.h"
#include "GameModeHMM.h"
#include "DataManager.h"
#include "CardConfigData.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;
using namespace UnderWorld::Core;

static const string default_value("0");

static Label* createLabel(Node* parent, const string& message)
{
    Label* label = CocosUtils::createLabel(message, DEFAULT_FONT_SIZE);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    parent->addChild(label);
    return label;
}

CardInfoNode* CardInfoNode::create(const Callback& callback)
{
    CardInfoNode *ret = new (nothrow) CardInfoNode();
    if (ret && ret->init(callback))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CardInfoNode::CardInfoNode()
:_cost(0)
,_callback(nullptr)
,_unit(nullptr)
,_cardNode(nullptr)
,_level(nullptr)
,_quality(nullptr)
,_population(nullptr)
,_description(nullptr)
,_hp(nullptr)
,_armor(nullptr)
,_armorPrefer(nullptr)
,_dmg(nullptr)
,_atkSpeed(nullptr)
,_atkRange(nullptr)
,_costLabel(nullptr) {}

CardInfoNode::~CardInfoNode()
{
    removeAllChildren();
}

bool CardInfoNode::init(const Callback& callback)
{
    if (Node::init()) {
        _callback = callback;
        
        static const Size size(420, 570);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        static const float edgeX(8.0f);
        static const float edgeY(8.0f);
        
        
        static const string file("GameImages/public/button_yellow.png");
        auto button = Button::create(file);
        button->addClickEventListener([this](Ref*) {
            if (_callback && _name.size() > 0) {
                _callback(_name, _cost);
            }
        });
        const auto& buttonSize(button->getContentSize());
        button->setPosition(Point(size.width / 2, edgeY + buttonSize.height / 2));
        addChild(button);
        
        _costLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        _costLabel->setPosition(Point(buttonSize.width / 2, buttonSize.height / 2));
        button->addChild(_costLabel);
        
        return true;
    }
    
    return false;
}

void CardInfoNode::update(const string& name)
{
    auto dm = DataManager::getInstance();
    const HMMCardType* ct = dm->getGameModeHMM()->findCardTypeByName(name);
    if (ct) {
        const UnitType* unit = dm->getTechTree()->findUnitTypeByName(name);
        if (unit) {
            _hp->setString(StringUtils::format("%d", unit->getMaxHp()));
            _armor->setString(StringUtils::format("%d", unit->getAmror()));
            
            const AttackSkillType* type = nullptr;
            for (int i = 0; i < FIELD_TYPE_COUNT; ++i) {
                for (int j = 0; j < UNIT_CLASS_COUNT; ++j) {
                    if (unit->getDefaultAttackSkillType((FieldType)i, (UnitClass)j)) {
                        type = dynamic_cast<const AttackSkillType*>(
                                                                    unit->getDefaultAttackSkillType((FieldType)i,
                                                                                                    (UnitClass)j));
                        break;
                    }
                }
            }
            
            if (type) {
                _dmg->setString(StringUtils::format("%d", (type->getMinDamage() + type->getMaxDamage()) / 2));
                _atkSpeed->setString(StringUtils::format("%d", GameConstants::frame2Second(type->getPrePerformFrames() + type->getCDFrames())));
                _atkRange->setString(StringUtils::format("%d", type->getRange()));
            } else {
                _dmg->setString(default_value);
                _atkSpeed->setString(default_value);
                _atkRange->setString(default_value);
            }
            
            const string& at = unit->getArmorPreference();
            if (at.length() > 0) {
                _armorPrefer->setString(StringUtils::format("%d", unit->getArmorPreferenceFactor().getLiteralValue()));
            } else {
                _armorPrefer->setString(default_value);
            }
        } else {
            _hp->setString(default_value);
            _armor->setString(default_value);
            _armorPrefer->setString(default_value);
            _dmg->setString(default_value);
            _atkSpeed->setString(default_value);
            _atkRange->setString(default_value);
        }
    } else {
        _description->setString("");
        _hp->setString(default_value);
        _armor->setString(default_value);
        _armorPrefer->setString(default_value);
        _dmg->setString(default_value);
        _atkSpeed->setString(default_value);
        _atkRange->setString(default_value);
    }
}
