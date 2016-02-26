//
//  MapUITalentNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/26.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUITalentNode.h"
#include "Camp.h"

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
        
        const Size size(button->getContentSize());
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        button->setPosition(Point(size.width / 2, size.height / 2));
        
        // TODO: add resource cost
        
        return true;
    }
    
    return false;
}

void MapUITalentNode::registerObserver(MapUITalentNodeObserver *observer)
{
    _observer = observer;
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
