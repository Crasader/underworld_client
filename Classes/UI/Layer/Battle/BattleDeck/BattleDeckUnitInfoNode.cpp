//
//  BattleDeckUnitInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleDeckUnitInfoNode.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosUtils.h"
#include "Unit.h"
#include "DataManager.h"
#include "CardConfigData.h"
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

BattleDeckUnitInfoNode* BattleDeckUnitInfoNode::create(const UnitBase* unit, int rarity)
{
    BattleDeckUnitInfoNode *ret = new (nothrow) BattleDeckUnitInfoNode();
    if (ret && ret->init(rarity))
    {
        ret->update(unit);
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleDeckUnitInfoNode::BattleDeckUnitInfoNode()
:_observer(nullptr)
,_unitIcon(nullptr)
,_qualityIcon(nullptr)
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
,_descriptionLabel(nullptr)
{
    
}

BattleDeckUnitInfoNode::~BattleDeckUnitInfoNode()
{
    removeAllChildren();
}

bool BattleDeckUnitInfoNode::init(int rarity)
{
    if (Node::init()) {
        static const string csbFile("UI_CardInfo.csb");
        Node *mainNode = CSLoader::createNode(csbFile);
        addChild(mainNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(csbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        Widget* root = dynamic_cast<Widget *>(mainNode->getChildByTag(60));
        root->setSwallowTouches(false);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                switch (tag) {
                    case 80:
                    {
                        static const string file("GameImages/icons/unit/icon_w_langdun_1.png");
                        _unitIcon = Sprite::create(file);
                        child->addChild(_unitIcon);
                    }
                        break;
                    case 81:
                    {
                        static const string file(StringUtils::format("GameImages/test/ui_kuang_%d.png", rarity + 4));
                        _qualityIcon = Sprite::create(file);
                        child->addChild(_qualityIcon);
                    }
                        break;
                    case 63:
                    {
                        static const string file("GameImages/icons/race/icon_langzu.png");
                        _raceIcon = Sprite::create(file);
                        child->addChild(_raceIcon);
                    }
                        break;
                    case 62:
                    {
                        _nameLabel = createLabel(child, "vampire");
                    }
                        break;
                    case 65:
                    {
                        Node* node = child->getChildByTag(66);
                        if (node) {
                            _hpLabel = createLabel(node, "0");
                        }
                    }
                        break;
                    case 67:
                    {
                        Node* node = child->getChildByTag(68);
                        if (node) {
                            _armorLabel = createLabel(node, "0");
                        }
                    }
                        break;
                    case 69:
                    {
                        Node* node = child->getChildByTag(70);
                        if (node) {
                            _armorPreferLabel = createLabel(node, "0");
                        }
                    }
                        break;
                    case 71:
                    {
                        Node* node = child->getChildByTag(72);
                        if (node) {
                            _dmgLabel = createLabel(node, "0");
                        }
                    }
                        break;
                    case 73:
                    {
                        Node* node = child->getChildByTag(74);
                        if (node) {
                            _atkSpeedLabel = createLabel(node, "0");
                        }
                    }
                        break;
                    case 75:
                    {
                        Node* node = child->getChildByTag(76);
                        if (node) {
                            _atkRangeLabel = createLabel(node, "0");
                        }
                    }
                        break;
                    case 21:
                    {
                        _skillLevelLabel = createLabel(child, "0");
                    }
                        break;
                    case 78:
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
                    case 79:
                    {
                        _descriptionLabel = CocosUtils::createLabel("description", DEFAULT_FONT_SIZE, DEFAULT_FONT, Size(130, 130));
                        _descriptionLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
                        child->addChild(_descriptionLabel);
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        
        const Size& size = root->getContentSize();
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        return true;
    }
    
    return false;
}

void BattleDeckUnitInfoNode::registerObserver(BattleDeckUnitInfoNodeObserver *observer)
{
    _observer = observer;
}

void BattleDeckUnitInfoNode::update(const UnitBase* unit)
{
    if (unit) {
        const string& file = DataManager::getInstance()->getCardConfigData(unit->getUnitType()->getName())->getSmallIcon();
        if (file.length() > 0 && FileUtils::getInstance()->isFileExist(file)) {
            _unitIcon->setTexture(file);
        }
        
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
                    if (_skillLevelLabel) {
                        _skillLevelLabel->setString(StringUtils::format("LV%d", pt->getLevel()));
                    }
                    if (_skillNameLabel) {
                        _skillNameLabel->setString(pt->getAlias());
                    }
                    _descriptionLabel->setString(pt->getDesc());
                }
            } else {
                const int spellTypeCount = unit->getSpellTypeCount();
                if (spellTypeCount > 0) {
                    const SpellType* st = unit->getSpellType(0);
                    if (st) {
                        if (_skillLevelLabel) {
                            _skillLevelLabel->setString(StringUtils::format("LV%d", st->getLevel()));
                        }
                        if (_skillNameLabel) {
                            _skillNameLabel->setString(st->getAlias());
                        }
                        _descriptionLabel->setString(st->getDesc());
                    }
                }
            }
        }
    }
}

void BattleDeckUnitInfoNode::update(const string& name, TechTree* techTree)
{
    const UnitType* unit = techTree->findUnitTypeByName(name);
    if (unit) {
        const string& file = DataManager::getInstance()->getCardConfigData(unit->getName())->getSmallIcon();
        if (file.length() > 0 && FileUtils::getInstance()->isFileExist(file)) {
            _unitIcon->setTexture(file);
        }
        
        _nameLabel->setString(unit->getName());
        _hpLabel->setString(StringUtils::format("%d", unit->getMaxHp()));
        _armorLabel->setString(StringUtils::format("%d", unit->getAmror()));
        
        const AttackSkillType* type = dynamic_cast<const AttackSkillType*>(unit->getDefaultAttackSkillType(kFieldType_Land));
        if (!type) {
            type = dynamic_cast<const AttackSkillType*>(unit->getDefaultAttackSkillType(kFieldType_Air));
        }
        
        if (type) {
            _dmgLabel->setString(StringUtils::format("%d", (type->getMinDamage() + type->getMaxDamage()) / 2));
            _atkSpeedLabel->setString(StringUtils::format("%.1f", GameConstants::frame2Second(type->getPrePerformFrames() + type->getCDFrames())));
            _atkRangeLabel->setString(StringUtils::format("%d", type->getRange()));
        } else {
            _dmgLabel->setString("");
            _atkSpeedLabel->setString("");
            _atkRangeLabel->setString("");
        }
        
        const string& at = unit->getArmorPreference();
        if (at.length() > 0) {
            _armorPreferLabel->setString(StringUtils::format("%.1f", unit->getArmorPreferenceFactor()));
        } else {
            _armorPreferLabel->setString("");
        }
        
        // skill
        {
            string name;
            const size_t passiveTypeCount = unit->getPassiveNames().size();
            if (passiveTypeCount > 0) {
                name = unit->getPassiveNames().at(0);
                
            } else {
                const size_t spellTypeCount = unit->getSpellNames().size();
                if (spellTypeCount > 0) {
                    name = unit->getSpellNames().at(0);
                }
            }
            
            if (name.length() > 0) {
                const SpellType* spellType = techTree->findSpellTypeByName(name);
                if (_skillLevelLabel) {
                    _skillLevelLabel->setString(StringUtils::format("LV%d", spellType ? spellType->getLevel() : 0));
                }
                if (_skillNameLabel) {
                    _skillNameLabel->setString(name);
                }
            } else {
                if (_skillLevelLabel) {
                    _skillLevelLabel->setString("");
                }
                if (_skillNameLabel) {
                    _skillNameLabel->setString("");
                }
            }
        }
        
        if (_descriptionLabel) {
            const CardType* ct = techTree->findCardTypeByName(name);
            _descriptionLabel->setString(ct ? ct->getDesc() : "");
        }
    }
}
