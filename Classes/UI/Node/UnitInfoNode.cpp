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

static LabelAtlas* createLabelAtlas(Node* parent, const string& message)
{
    LabelAtlas* label = CocosUtils::create10x25Number(message);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    parent->addChild(label);
    return label;
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

UnitInfoNode* UnitInfoNode::create(const Unit* unit)
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
,_dmgRateLabel(nullptr)
,_dmgLabel(nullptr)
,_dpsLabel(nullptr)
,_rangeLabel(nullptr)
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
        Node* topRoot = topNode->getChildByTag(2);
        
        Node* bottomNode = CSLoader::createNode("UI_HeroSkillInfo.csb");
        addChild(bottomNode);
        Node* bottomRoot = bottomNode->getChildByTag(19);
        
        const Size& topSize = topRoot->getContentSize();
        const Size& bottomSize = bottomRoot->getContentSize();
        const Size size(MAX(topSize.width, bottomSize.width), topSize.height + bottomSize.height);
        setContentSize(size);
        
        topNode->setPosition(Point(0, bottomSize.height / 2));
        bottomNode->setPosition(Point(0, - topSize.height / 2));
        
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
                                _hpLabel = createLabelAtlas(child, "0");
                            }
                                break;
                            case 13:
                            {
                                _armorLabel = createLabelAtlas(child, "0");
                            }
                                break;
                            case 14:
                            {
                                _dmgRateLabel = createLabelAtlas(child, "0");
                            }
                                break;
                            case 15:
                            {
                                _dmgLabel = createLabelAtlas(child, "0");
                            }
                                break;
                            case 16:
                            {
                                _dpsLabel = createLabelAtlas(child, "0");
                            }
                                break;
                            case 17:
                            {
                                _rangeLabel = createLabelAtlas(child, "0");
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
                                                    static const string file("GameImages/icons/race/icon_langzu.png");
                                                    _skillIcon = Sprite::create(file);
                                                    child->addChild(_skillIcon);
                                                }
                                                    break;
                                                case 22:
                                                {
                                                    _skillLevelLabel = createLabelAtlas(child, "0");
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
                                _skillDescriptionLabel = createLabel(child, "skill description");
                                _skillDescriptionLabel->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
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

void UnitInfoNode::update(const Unit* unit)
{
    _unit = unit;
    assert(!_data);
    if (_unit) {
        
    }
}

void UnitInfoNode::update(nullptr_t nullp)
{}
