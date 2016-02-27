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
,_resourceNode(nullptr)
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
        
        _resourceNode = BattleSmallResourceNode::create(kResourceType_Gold, 1);
        addChild(_resourceNode);
        
        const Size& buttonSize(button->getContentSize());
        static const Size nodeSize(30, 30);
        const Size size(buttonSize + nodeSize / 2);
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        button->setPosition(Point((buttonSize.width + nodeSize.width) / 2, buttonSize.height / 2));
        _resourceNode->setPosition(Point(nodeSize.width / 2, buttonSize.height));        
        
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
    if (_resourceNode) {
        _resourceNode->check(gold);
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
    return StringUtils::format("GameImages/test/icon_j_%d.png", _idx);
}
