//
//  UnitInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/8.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UnitInfoNode.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "SoldierData.h"
#include "Unit.h"
#include "SoundManager.h"

using namespace std;
using namespace ui;
using namespace cocostudio;
using namespace UnderWorld::Core;

static Label* createLabel(Node* parent, const string& message)
{
    Label* label = CocosUtils::createLabel(message, DEFAULT_FONT_SIZE);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    parent->addChild(label);
    return label;
}

static float getUnitAttributeValue(const UnitBase* unitBase, UnitAttributeType type)
{
    const int cnt = unitBase->getAttributeCount(type);
    if (cnt > 0) {
        const UnitAttribute& attribute = unitBase->getUnitAttribute(type, 0);
        return attribute._baseValue;
    }
    
    return .0f;
}

UnitInfoNode* UnitInfoNode::create(const AbstractData* data)
{
    UnitInfoNode *ret = new (nothrow) UnitInfoNode();
    if (ret && ret->init())
    {
        ret->update(data);
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitInfoNode* UnitInfoNode::create(const UnitBase* unit)
{
    UnitInfoNode *ret = new (nothrow) UnitInfoNode();
    if (ret && ret->init())
    {
        ret->update(unit);
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitInfoNode* UnitInfoNode::create(nullptr_t nullp)
{
    UnitInfoNode *ret = new (nothrow) UnitInfoNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitInfoNode::UnitInfoNode()
:_observer(nullptr)
,_data(nullptr)
,_raceIcon(nullptr)
,_nameLabel(nullptr)
,_hpLabel(nullptr)
,_armorLabel(nullptr)
,_armorPreferLabel(nullptr)
,_dmgLabel(nullptr)
,_atkSpeedLabel(nullptr)
,_atkRangeLabel(nullptr)
,_skillIcon(nullptr)
,_skillLevelLabel(nullptr)
,_skillNameLabel(nullptr)
,_skillDescriptionLabel(nullptr)
{
    
}

UnitInfoNode::~UnitInfoNode()
{
    removeAllChildren();
}

bool UnitInfoNode::init()
{
    if (Node::init()) {
        
        Node* topNode = CSLoader::createNode("UI_HeroInfo.csb");
        addChild(topNode);
        Widget* topRoot = dynamic_cast<Widget*>(topNode->getChildByTag(2));
        topRoot->setSwallowTouches(false);
        
        Node* bottomNode = CSLoader::createNode("UI_HeroSkillInfo.csb");
        addChild(bottomNode);
        Widget* bottomRoot = dynamic_cast<Widget*>(bottomNode->getChildByTag(19));
        bottomRoot->setSwallowTouches(false);
        
        const Size& topSize = topRoot->getContentSize();
        const Size& bottomSize = bottomRoot->getContentSize();
        const Size size(MAX(topSize.width, bottomSize.width), topSize.height + bottomSize.height);
        setContentSize(size);
        
        topNode->setPosition(Point(size.width / 2, (size.height + bottomSize.height) / 2));
        bottomNode->setPosition(Point(size.width / 2, (size.height - topSize.height) / 2));
        
        // top node
        {
            const Vector<Node*>& children = topRoot->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                Node* child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 3:
                            {
                                static const string file("GameImages/icons/race/icon_langzu.png");
                                _raceIcon = Sprite::create(file);
                                child->addChild(_raceIcon);
                            }
                                break;
                            case 4:
                            {
                                _nameLabel = createLabel(child, "vampire");
                            }
                                break;
                            case 12:
                            {
                                _hpLabel = createLabel(child, "0");
                            }
                                break;
                            case 13:
                            {
                                _armorLabel = createLabel(child, "0");
                            }
                                break;
                            case 14:
                            {
                                _armorPreferLabel = createLabel(child, "0");
                            }
                                break;
                            case 15:
                            {
                                _dmgLabel = createLabel(child, "0");
                            }
                                break;
                            case 16:
                            {
                                _atkSpeedLabel = createLabel(child, "0");
                            }
                                break;
                            case 17:
                            {
                                _atkRangeLabel = createLabel(child, "0");
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        // bottom node
        {
            const Vector<Node*>& children = bottomRoot->getChildren();
            for (int i = 0; i < children.size(); ++i)
            {
                Node* child = children.at(i);
                if (child) {
                    const int tag = child->getTag();
                    if (tag > 0) {
                        switch (tag) {
                            case 20:
                            {
                                const Vector<Node*>& children = child->getChildren();
                                for (int i = 0; i < children.size(); ++i)
                                {
                                    Node* child = children.at(i);
                                    if (child) {
                                        const int tag = child->getTag();
                                        if (tag > 0) {
                                            switch (tag) {
                                                case 21:
                                                {
                                                    _skillLevelLabel = createLabel(child, "0");
                                                }
                                                    break;
                                                case 22:
                                                {
                                                    static const string file("GameImages/icons/race/icon_langzu.png");
                                                    _skillIcon = Sprite::create(file);
                                                    child->addChild(_skillIcon);
                                                }
                                                    break;
                                                case 23:
                                                {
                                                    _skillNameLabel = createLabel(child, "skill");
                                                }
                                                    break;
                                                default:
                                                    break;
                                            }
                                        }
                                    }
                                }
                            }
                                break;
                            case 24:
                            {
                                _skillDescriptionLabel = CocosUtils::createLabel("skill description", DEFAULT_FONT_SIZE, Size(170, 130));
                                _skillDescriptionLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
                                child->addChild(_skillDescriptionLabel);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
        
        return true;
    }
    
    return false;
}

void UnitInfoNode::registerObserver(UnitInfoNodeObserver *observer)
{
    _observer = observer;
}

void UnitInfoNode::update(const AbstractData* data)
{
    _data = data;
    assert(!_unit);
    if (data) {
        
    }
}

void UnitInfoNode::update(const UnitBase* unit)
{
    _unit = unit;
    assert(!_data);
    
    if (unit) {
        _nameLabel->setString(unit->getUnitName());
        _hpLabel->setString(StringUtils::format("%.0f", getUnitAttributeValue(unit, kUnitAttribute_MaxHp)));
        _armorLabel->setString(StringUtils::format("%.0f", getUnitAttributeValue(unit, kUnitAttribute_Armor)));
        _dmgLabel->setString(StringUtils::format("%.0f", getUnitAttributeValue(unit, kUnitAttribute_AttackDamage)));
        _atkSpeedLabel->setString(StringUtils::format("%.1f", getUnitAttributeValue(unit, kUnitAttribute_AttackSpeed)));
        _atkRangeLabel->setString(StringUtils::format("%.1f", getUnitAttributeValue(unit, kUnitAttribute_AttackRange)));
        
        const ArmorType* at = unit->getArmorPreference();
        if (at) {
            _armorPreferLabel->setString(StringUtils::format("%.1f", unit->getArmorPreferencFactor()));
        }
        
        // skill
        {
            const int passiveTypeCount = unit->getPassiveTypeCount();
            if (passiveTypeCount > 0) {
                const PassiveType* pt = unit->getPassiveType(0);
                if (pt) {
                    _skillLevelLabel->setString(StringUtils::format("LV%d", pt->getLevel()));
                    _skillNameLabel->setString(pt->getAlias());
                    _skillDescriptionLabel->setString(pt->getDesc());
                }
            } else {
                const int spellTypeCount = unit->getSpellTypeCount();
                if (spellTypeCount > 0) {
                    const SpellType* st = unit->getSpellType(0);
                    if (st) {
                        _skillLevelLabel->setString(StringUtils::format("LV%d", st->getLevel()));
                        _skillNameLabel->setString(st->getAlias());
                        _skillDescriptionLabel->setString(st->getDesc());
                    }
                }
            }
        }
    }
}

void UnitInfoNode::update(nullptr_t nullp)
{}
