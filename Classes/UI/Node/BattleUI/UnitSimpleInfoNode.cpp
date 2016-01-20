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
#include "DataManager.h"
#include "SoldierLocalData.h"
#include "URConfigData.h"
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

UnitSimpleInfoNode* UnitSimpleInfoNode::create(const Camp* camp, const UnitBase* unit)
{
    UnitSimpleInfoNode *ret = new (nothrow) UnitSimpleInfoNode();
    if (ret && ret->init())
    {
        ret->update(camp, unit);
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

UnitSimpleInfoNode::UnitSimpleInfoNode()
:_observer(nullptr)
,_data(nullptr)
,_camp(nullptr)
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
        
        Widget* root = dynamic_cast<Widget*>(node->getChildByTag(27));
        if (root) {
            root->setSwallowTouches(false);
            const Size& size = root->getContentSize();
            node->setPosition(Point(size.width / 2, size.height / 2));
            setContentSize(size);
            root->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
                if (_observer) {
                    _observer->onUnitSimpleInfoNodeTouched(pSender, type, _unit);
                }
            });
        }
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
                            static const string file("GameImages/icons/unit/small/icon_v_nvgongjianshou_1.png");
                            _icon = Sprite::create(file);
                            child->addChild(_icon);
                        }
                            break;
                        case 29:
                        {
                            const Vector<Node*>& children = child->getChildren();
                            for (int i = 0; i < children.size(); ++i)
                            {
                                Node* child = children.at(i);
                                if (child) {
                                    const int tag = child->getTag();
                                    if (tag > 0) {
                                        switch (tag) {
                                            case 30:
                                            {
                                                _countLabel = createLabelAtlas(child, "0/2");
                                                _countLabel->setAnchorPoint(Point::ANCHOR_MIDDLE);
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
                            _levelLabel = createLabelAtlas(child, "&'1");
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
    assert(!_camp && !_unit);
    if (data) {
        
    }
}

void UnitSimpleInfoNode::update(const Camp* camp, const UnitBase* unit)
{
    _camp = camp;
    _unit = unit;
    assert(!_data);
    if (camp) {
        _countLabel->setString(StringUtils::format("%d/%d", camp->getProduction(), camp->getMaxProduction()));
        const string& file = DataManager::getInstance()->getURConfigData(camp->getUnitType()->getRenderKey())->getSmallIcon();
        if (FileUtils::getInstance()->isFileExist(file)) {
            _icon->setTexture(file);
        }
    }
    
    if (unit) {
        _levelLabel->setString(StringUtils::format("&'%d", unit->getLevel()));
    }
}
