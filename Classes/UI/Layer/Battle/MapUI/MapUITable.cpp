//
//  MapUITable.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/27.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUITable.h"
#include "Camp.h"

using namespace std;
using namespace UnderWorld::Core;

MapUITable* MapUITable::create(const vector<const Camp*>& camps)
{
    MapUITable *ret = new (nothrow) MapUITable();
    if (ret && ret->init(camps))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MapUITable::MapUITable()
:_observer(nullptr)
{
    
}

MapUITable::~MapUITable()
{
    removeAllChildren();
}

bool MapUITable::init(const vector<const Camp*>& camps)
{
    if (Node::init())
    {
        const size_t cnt = camps.size();
        Size nodeSize(Size::ZERO);
        for (int i = 0; i < cnt; ++i) {
            MapUIUnitNode* node = MapUIUnitNode::create(camps.at(i));
            node->registerObserver(this);
            if (0 == nodeSize.width) {
                nodeSize = node->getContentSize();
            }
            _unitNodes.push_back(node);
        }
        
        // calculate size
        if (cnt > 0) {
            static const float offset(5.0f);
            Rect rect(0, 0, 91, 157);
            static const float capInsets(18.0f);
            Rect capInsetsRect(capInsets, capInsets, rect.size.width - capInsets * 2, rect.size.height - capInsets * 2);
            Scale9Sprite* background = Scale9Sprite::create("GameImages/test/ui_black_13.png", rect, capInsetsRect);
            addChild(background);
            
            const float nodeWidth(nodeSize.width);
            const Size size((nodeWidth + offset) * cnt + offset, nodeSize.height + offset * 2);
            background->setContentSize(size);
            for (int i = 0; i < _unitNodes.size(); ++i) {
                auto node = _unitNodes.at(i);
                background->addChild(node);
                node->setPosition(Point((nodeWidth + offset) * (i + 1) - nodeWidth / 2, size.height / 2));
            }
            
            setAnchorPoint(Point::ANCHOR_MIDDLE);
            setContentSize(size);
            background->setPosition(Point(size.width / 2, size.height / 2));
        }
        
        return true;
    }
    
    return false;
}

void MapUITable::registerObserver(MapUITableObserver *observer)
{
    _observer = observer;
}

void MapUITable::reload(float gold, float wood, const Camp* selectedCamp)
{
    for (auto& node : _unitNodes) {
        node->update(gold, wood);
        node->setSelected(selectedCamp == node->getCamp());
    }
}

#pragma mark - MapUIUnitNodeObserver
void MapUITable::onMapUIUnitNodeClickedAddButton(const Camp* camp)
{
    if (_observer) {
        _observer->onMapUITableUnitAdd(camp);
    }
}

void MapUITable::onMapUIUnitNodeClickedUpgradeButton(MapUIUnitNode* node)
{
    if (_observer) {
        _observer->onMapUITableUnitUpgrade(node);
    }
}

void MapUITable::onMapUIUnitNodeTouchedBegan(const Camp* camp)
{
    if (_observer) {
        _observer->onMapUITableUnitTouchedBegan(camp);
    }
}

void MapUITable::onMapUIUnitNodeTouchedEnded(const Camp* camp, bool isValid)
{
    if (isValid && _observer) {
        _observer->onMapUITableUnitTouchedEnded(camp);
    }
}
