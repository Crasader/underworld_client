//
//  CardInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/24.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardInfoNode.h"
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
,_atkType(nullptr)
,_dmg(nullptr)
,_atkSpeed(nullptr)
,_atkRange(nullptr)
,_button(nullptr) {}

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
        
        // add lines
        vector<Sprite*> lines;
        for (int i = 0; i < 2; ++i) {
            auto line = Sprite::create("GameImages/public/ui_line.png");
            line->setScaleX(size.width / line->getContentSize().width);
            addChild(line);
            
            lines.push_back(line);
        }
        
        static const float edgeX(8.0f);
        static const float edgeY(8.0f);
        _cardNode = CardNode::create(false);
        addChild(_cardNode);
        
        addLabel(&_level);
        addLabel(&_quality);
        addLabel(&_population);
        addLabel(&_description);
        
        const auto& cardSize(_cardNode->getContentSize());
        _cardNode->setPosition(Point(edgeX + cardSize.width / 2, size.height - (edgeY + cardSize.height / 2)));
        lines.at(0)->setPosition(Point(size.width / 2, _cardNode->getPosition().y - (edgeY + cardSize.height / 2 + 8.0f)));
        {
            const auto lh(_level->getContentSize().height);
            const auto qh(_quality->getContentSize().height);
            const auto ph(_population->getContentSize().height);
            const auto dh(_description->getContentSize().height);
            _level->setPosition(Point(edgeX + cardSize.width + edgeX, size.height - (edgeY + lh / 2)));
            _quality->setPosition(_level->getPosition() - Point(0, (lh + qh) / 2));
            _population->setPosition(_quality->getPosition() - Point(0, (qh + ph) / 2));
            _description->setPosition(_population->getPosition() - Point(0, (ph + dh) / 2));
        }
        
        addLabel(&_hp);
        addLabel(&_armor);
        addLabel(&_atkType);
        
        {
            const auto hh(_hp->getContentSize().height);
            const auto ah(_armor->getContentSize().height);
            const auto ph(_atkType->getContentSize().height);
            _hp->setPosition(Point(edgeX, lines.at(0)->getPosition().y - (edgeY + hh) / 2));
            _armor->setPosition(_hp->getPosition() - Point(0, edgeY + (hh + ah) / 2));
            _atkType->setPosition(_armor->getPosition() - Point(0, edgeY + (ah + ph) / 2));
        }
        
        addLabel(&_dmg);
        addLabel(&_atkSpeed);
        addLabel(&_atkRange);
        
        {
            const float x(size.width * 0.6f);
            _dmg->setPosition(Point(x, _hp->getPosition().y));
            _atkSpeed->setPosition(Point(x, _armor->getPosition().y));
            _atkRange->setPosition(Point(x, _atkType->getPosition().y));
        }
        
        static const string file("GameImages/public/button_yellow.png");
        auto button = Button::create(file);
        button->addClickEventListener([this](Ref*) {
            if (_callback && _name.size() > 0) {
                _callback(_name, _cost);
            }
        });
        const auto& buttonSize(button->getContentSize());
        button->setPosition(Point(size.width / 2, edgeY + buttonSize.height / 2));
        button->setTitleFontName(DEFAULT_FONT);
        button->setTitleFontSize(DEFAULT_FONT_SIZE);
        addChild(button);
        _button = button;
        
        lines.at(1)->setPosition(Point(size.width / 2, button->getPosition().y + buttonSize.height / 2 + edgeY));
        updateCost(0);
        
        return true;
    }
    
    return false;
}

void CardInfoNode::update(const string& name)
{
    if (_cardNode) {
        _cardNode->update(name, DataManager::getInstance()->getBattleResourceMaxCount());
    }
    
    auto dm = DataManager::getInstance();
    auto ct = dm->getGameModeHMM()->findCardTypeByName(name);
    if (ct) {
        auto unit = dm->getTechTree()->findUnitTypeByName(name);
        if (unit) {
            _hp->setString(StringUtils::format("%d", unit->getMaxHp()));
            _armor->setString(StringUtils::format("%d", unit->getAmror()));
            _atkType->setString(unit->getAttackType()->getName());
            
            const AttackSkillType* type = nullptr;
            for (int i = 0; i < FIELD_TYPE_COUNT; ++i) {
                for (int j = 0; j < UNIT_CLASS_COUNT; ++j) {
                    if (unit->getDefaultAttackSkillType((FieldType)i, (UnitClass)j)) {
                        type = dynamic_cast<const AttackSkillType*>(unit->getDefaultAttackSkillType((FieldType)i, (UnitClass)j));
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
        } else {
            _hp->setString(default_value);
            _armor->setString(default_value);
            _atkType->setString(default_value);
            _dmg->setString(default_value);
            _atkSpeed->setString(default_value);
            _atkRange->setString(default_value);
        }
    } else {
        _description->setString("");
        _hp->setString(default_value);
        _armor->setString(default_value);
        _atkType->setString(default_value);
        _dmg->setString(default_value);
        _atkSpeed->setString(default_value);
        _atkRange->setString(default_value);
    }
}

void CardInfoNode::updateCost(int count)
{
    _cost = count;
    if (_button) {
        _button->setTitleText(StringUtils::format("Cost: %d", count));
    }
}

void CardInfoNode::addLabel(Label** label)
{
    *label = CocosUtils::createLabel("0", BIG_FONT_SIZE);
    (*label)->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    addChild(*label);
}
