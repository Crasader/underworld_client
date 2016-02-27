//
//  MapUIUpgradeNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/2/26.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUIUpgradeNode.h"
#include "Camp.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "MapUITalentNode.h"

using namespace std;
using namespace UnderWorld::Core;

MapUIUpgradeNode* MapUIUpgradeNode::create(const Camp* camp)
{
    MapUIUpgradeNode *ret = new (nothrow) MapUIUpgradeNode();
    if (ret && ret->init(camp))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MapUIUpgradeNode::MapUIUpgradeNode()
:_observer(nullptr)
,_camp(nullptr)
{
    
}

MapUIUpgradeNode::~MapUIUpgradeNode()
{
    removeAllChildren();
}

bool MapUIUpgradeNode::init(const Camp* camp)
{
    if (Node::init())
    {
        _camp = camp;
        
        const int cnt = camp->getUpgradeCount();
        Size nodeSize(Size::ZERO);
        for (int i = 0; i < cnt; ++i) {
            MapUITalentNode* node = MapUITalentNode::create(camp, i);
            node->registerObserver(this);
            if (0 == nodeSize.width) {
                nodeSize = node->getContentSize();
            }
            _nodes.push_back(node);
        }
        
        // calculate size
        if (cnt > 0) {
            static const float offsetX(5.0f);
            static const float offsetY(5.0f);
            const float nodeWidth(nodeSize.width);
            const Size size((nodeWidth + offsetX) * cnt + offsetX, nodeSize.height + offsetY * 2);
            Sprite* background = CocosUtils::createPureColorSprite(size, LAYER_MASK_COLOR);
            addChild(background);
            for (int i = 0; i < _nodes.size(); ++i) {
                auto node = _nodes.at(i);
                background->addChild(node);
                node->setPosition(Point((nodeWidth + offsetX) * (i + 1) - nodeWidth / 2, size.height / 2));
            }
            
            setAnchorPoint(Point::ANCHOR_MIDDLE);
            setContentSize(size);
            background->setPosition(Point(size.width / 2, size.height / 2));
        }
        
        return true;
    }
    
    return false;
}

void MapUIUpgradeNode::registerObserver(MapUIUpgradeNodeObserver *observer)
{
    _observer = observer;
}

void MapUIUpgradeNode::check(float gold, float wood)
{
    for (int i = 0; i < _nodes.size(); ++i) {
        _nodes.at(i)->check(gold, wood);
    }
}

const Camp* MapUIUpgradeNode::getCamp() const
{
    return _camp;
}

#pragma mark - MapUITalentNodeObserver
void MapUIUpgradeNode::onMapUITalentNodeClickedButton(MapUITalentNode* node)
{
    if (_observer) {
        _observer->onMapUIUpgradeNodeClickedButton(node->getCamp(), node->getIdx());
    }
}
