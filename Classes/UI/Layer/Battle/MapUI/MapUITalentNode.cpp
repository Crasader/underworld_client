//
//  MapUITalentNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/26.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUITalentNode.h"
#include "ui/CocosGUI.h"
#include "Camp.h"
#include "BattleSmallResourceNode.h"

using namespace std;
using namespace UnderWorld::Core;

MapUITalentNode* MapUITalentNode::create(const Camp* camp, int idx)
{
    MapUITalentNode *ret = new (nothrow) MapUITalentNode();
    if (ret && ret->init(camp, idx))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MapUITalentNode::MapUITalentNode()
:_observer(nullptr)
,_camp(nullptr)
,_idx(INVALID_VALUE)
,_goldNode(nullptr)
,_woodNode(nullptr)
{
    
}

MapUITalentNode::~MapUITalentNode()
{
    removeAllChildren();
}

bool MapUITalentNode::init(const Camp* camp, int idx)
{
    if (Node::init())
    {
        _camp = camp;
        _idx = idx;
        
        const string& iconFile = getIconFile(true);
        Button* button = Button::create(iconFile, iconFile);
        addChild(button);
        button->setPressedActionEnabled(true);
        button->setEnabled(camp->canUpgrade(idx));
        button->addClickEventListener([this](Ref*) {
            if (_observer) {
                _observer->onMapUITalentNodeClickedButton(this);
            }
        });
        
        const map<string, int>& costs = camp->getUpCosts(idx);
        if (costs.find(RES_NAME_GOLD) != costs.end()) {
            _goldNode = BattleSmallResourceNode::create(kResourceType_Gold, costs.at(RES_NAME_GOLD));
            addChild(_goldNode);
        }
        
        if (costs.find(RES_NAME_WOOD) != costs.end()) {
            _woodNode = BattleSmallResourceNode::create(kResourceType_Wood, costs.at(RES_NAME_WOOD));
            addChild(_woodNode);
        }
        
        const Size& buttonSize(button->getContentSize());
        static const Size nodeSize(30, 30);
        const Size size(buttonSize + nodeSize - Size(0, nodeSize.height / 2));
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        button->setPosition(Point(size.width / 2, buttonSize.height / 2));
        if (_goldNode) {
            _goldNode->setPosition(Point(nodeSize.width / 2, buttonSize.height));
        }
        if (_woodNode) {
            _woodNode->setPosition(Point(size.width - nodeSize.width / 2, buttonSize.height));
        }
        
        return true;
    }
    
    return false;
}

void MapUITalentNode::registerObserver(MapUITalentNodeObserver *observer)
{
    _observer = observer;
}

void MapUITalentNode::check(float gold, float wood)
{
    if (_goldNode) {
        _goldNode->check(gold);
    }
    
    if (_woodNode) {
        _woodNode->check(wood);
    }
}

const Camp* MapUITalentNode::getCamp() const
{
    return _camp;
}

int MapUITalentNode::getIdx() const
{
    return _idx;
}

string MapUITalentNode::getIconFile(bool enable) const
{
    return StringUtils::format("GameImages/test/icon_j_%d.png", _idx < 3 ? _idx : 2);
}
