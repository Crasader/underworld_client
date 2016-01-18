//
//  UnitSimpleInfoNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UnitSimpleInfoNode.h"
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

static LabelAtlas* createLabelAtlas(Node* parent, const string& message)
{
    LabelAtlas* label = CocosUtils::create10x25Number(message);
    label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    parent->addChild(label);
    return label;
}

UnitSimpleInfoNode* UnitSimpleInfoNode::create(const AbstractData* data)
{
    UnitSimpleInfoNode *ret = new (nothrow) UnitSimpleInfoNode();
    if (ret && ret->init())
    {
        ret->update(data);
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitSimpleInfoNode* UnitSimpleInfoNode::create(const Unit* unit)
{
    UnitSimpleInfoNode *ret = new (nothrow) UnitSimpleInfoNode();
    if (ret && ret->init())
    {
        ret->update(unit);
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitSimpleInfoNode* UnitSimpleInfoNode::create(nullptr_t nullp)
{
    UnitSimpleInfoNode *ret = new (nothrow) UnitSimpleInfoNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitSimpleInfoNode::UnitSimpleInfoNode()
:_observer(nullptr)
,_data(nullptr)
,_unit(nullptr)
,_icon(nullptr)
,_countLabel(nullptr)
,_levelLabel(nullptr)
{
    
}

UnitSimpleInfoNode::~UnitSimpleInfoNode()
{
    removeAllChildren();
}

bool UnitSimpleInfoNode::init()
{
    if (Node::init()) {
        
        Node* node = CSLoader::createNode("UI_BattleIcon.csb");
        addChild(node);
        
        Node* root = node->getChildByTag(27);
        setContentSize(root->getContentSize());
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                        case 28:
                        {
                            static const string file("GameImages/icons/race/icon_langzu.png");
                            _icon = Sprite::create(file);
                            child->addChild(_icon);
                        }
                            break;
                        case 29:
                        {
                            const Vector<Node*>& children = root->getChildren();
                            for (int i = 0; i < children.size(); ++i)
                            {
                                Node* child = children.at(i);
                                if (child) {
                                    const int tag = child->getTag();
                                    if (tag > 0) {
                                        switch (tag) {
                                            case 30:
                                            {
                                                _countLabel = createLabelAtlas(child, "0");
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
                        case 31:
                        {
                            _levelLabel = createLabelAtlas(child, "0");
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        return true;
    }
    
    return false;
}

void UnitSimpleInfoNode::registerObserver(UnitSimpleInfoNodeObserver *observer)
{
    _observer = observer;
}

void UnitSimpleInfoNode::update(const AbstractData* data)
{
    _data = data;
    assert(!_unit);
    if (data) {
        
    }
}

void UnitSimpleInfoNode::update(const Unit* unit)
{
    _unit = unit;
    assert(!_data);
    if (_unit) {
        
    }
}

void UnitSimpleInfoNode::update(nullptr_t nullp)
{}
