//
//  BattleDeckUnitInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleDeckUnitInfoNode.h"
#include "ui/CocosGUI.h"
#include "CocosUtils.h"
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

BattleDeckUnitInfoNode* BattleDeckUnitInfoNode::create()
{
    BattleDeckUnitInfoNode *ret = new (nothrow) BattleDeckUnitInfoNode();
    if (ret && ret->init())
    {
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
,_descriptionLabel(nullptr)
{
    
}

BattleDeckUnitInfoNode::~BattleDeckUnitInfoNode()
{
    removeAllChildren();
}

bool BattleDeckUnitInfoNode::init()
{
    if (Node::init()) {
        static const string csbFile("UI_CardInfo.csb");
        auto mainNode = CocosUtils::playCSBAnimation(csbFile, false, 0, nullptr);
        addChild(mainNode);
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
                        _unitIcon = Sprite::create();
                        child->addChild(_unitIcon);
                    }
                        break;
                    case 81:
                    {
                        _qualityIcon = Sprite::create();
                        child->addChild(_qualityIcon);
                    }
                        break;
                    case 63:
                    {
                        _raceIcon = Sprite::create();
                        child->addChild(_raceIcon);
                    }
                        break;
                    case 62:
                    {
                        _nameLabel = createLabel(child, "");
                    }
                        break;
                    case 65:
                    {
                        Node* node = child->getChildByTag(66);
                        if (node) {
                            _hpLabel = createLabel(node, default_value);
                        }
                    }
                        break;
                    case 67:
                    {
                        Node* node = child->getChildByTag(68);
                        if (node) {
                            _armorLabel = createLabel(node, default_value);
                        }
                    }
                        break;
                    case 69:
                    {
                        Node* node = child->getChildByTag(70);
                        if (node) {
                            _armorPreferLabel = createLabel(node, default_value);
                        }
                    }
                        break;
                    case 71:
                    {
                        Node* node = child->getChildByTag(72);
                        if (node) {
                            _dmgLabel = createLabel(node, default_value);
                        }
                    }
                        break;
                    case 73:
                    {
                        Node* node = child->getChildByTag(74);
                        if (node) {
                            _atkSpeedLabel = createLabel(node, default_value);
                        }
                    }
                        break;
                    case 75:
                    {
                        Node* node = child->getChildByTag(76);
                        if (node) {
                            _atkRangeLabel = createLabel(node, default_value);
                        }
                    }
                        break;
                    case 78:
                    {
                        static const string file("GameImages/icons/race/icon_langzu.png");
                        _skillIcon = Sprite::create(file);
                        child->addChild(_skillIcon);
                    }
                        break;
                    case 79:
                    {
                        _descriptionLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE, DEFAULT_FONT, Size(130, 130));
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

void BattleDeckUnitInfoNode::update(const string& name, TechTree* techTree, GameModeHMM* gameModeHMM)
{
    const HMMCardType* ct = gameModeHMM->findCardTypeByName(name);
    if (ct) {
        const string& file = DataManager::getInstance()->getCardConfigData(name)->getSmallIcon();
        if (file.length() > 0 && FileUtils::getInstance()->isFileExist(file)) {
            _unitIcon->setVisible(true);
            _unitIcon->setTexture(file);
        } else {
            _unitIcon->setVisible(false);
        }
        
        _raceIcon->setTexture("GameImages/icons/race/icon_langzu.png");
        _nameLabel->setString(name);
//        _descriptionLabel->setString(ct->getDesc());
        
        const UnitType* unit = techTree->findUnitTypeByName(name);
        _qualityIcon->setVisible(unit);
        if (unit) {
            _qualityIcon->setTexture(StringUtils::format("GameImages/test/ui_kuang_%d.png", unit->getRarity() + 4));
            _hpLabel->setString(StringUtils::format("%d", unit->getMaxHp()));
            _armorLabel->setString(StringUtils::format("%d", unit->getAmror()));
            
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
                _dmgLabel->setString(StringUtils::format("%d", (type->getMinDamage() + type->getMaxDamage()) / 2));
                _atkSpeedLabel->setString(StringUtils::format("%d", GameConstants::frame2Second(type->getPrePerformFrames() + type->getCDFrames())));
                _atkRangeLabel->setString(StringUtils::format("%d", type->getRange()));
            } else {
                _dmgLabel->setString(default_value);
                _atkSpeedLabel->setString(default_value);
                _atkRangeLabel->setString(default_value);
            }
            
            const string& at = unit->getArmorPreference();
            if (at.length() > 0) {
                _armorPreferLabel->setString(StringUtils::format("%d", unit->getArmorPreferenceFactor().getLiteralValue()));
            } else {
                _armorPreferLabel->setString(default_value);
            }
            
            // skill
            {
                string name;
                const size_t passiveTypeCount = unit->getPassiveTypeCount();
                if (passiveTypeCount > 0) {
                    name = unit->getPassiveTypeByIndex(0)->getName();
                    
                } else {
                    const size_t spellTypeCount = unit->getSpellTypeCount();
                    if (spellTypeCount > 0) {
                        name = unit->getSpellTypeByIndex(0)->getSpellName();
                    }
                }
                
                if (name.length() > 0) {
                    
                } else {
                    
                }
            }
        } else {
            _hpLabel->setString(default_value);
            _armorLabel->setString(default_value);
            _armorPreferLabel->setString(default_value);
            _dmgLabel->setString(default_value);
            _atkSpeedLabel->setString(default_value);
            _atkRangeLabel->setString(default_value);
        }
    } else {
        _unitIcon->setVisible(false);
        _qualityIcon->setVisible(false);
        _nameLabel->setString(name);
        _descriptionLabel->setString("");
        _hpLabel->setString(default_value);
        _armorLabel->setString(default_value);
        _armorPreferLabel->setString(default_value);
        _dmgLabel->setString(default_value);
        _atkSpeedLabel->setString(default_value);
        _atkRangeLabel->setString(default_value);
    }
}
