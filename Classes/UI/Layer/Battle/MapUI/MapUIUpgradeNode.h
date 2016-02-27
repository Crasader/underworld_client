//
//  MapUIUpgradeNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/26.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUIUpgradeNode_h
#define MapUIUpgradeNode_h

#include "cocos2d.h"
#include "MapUITalentNode.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core {
    class Camp;
}}

class MapUIUpgradeNode;

class MapUIUpgradeNodeObserver
{
public:
    virtual ~MapUIUpgradeNodeObserver() {}
    virtual void onMapUIUpgradeNodeClickedButton(const UnderWorld::Core::Camp* camp, int idx) = 0;
};

class MapUIUpgradeNode: public Node, public MapUITalentNodeObserver
{
public:
    static MapUIUpgradeNode* create(const UnderWorld::Core::Camp* camp);
    virtual ~MapUIUpgradeNode();
    void registerObserver(MapUIUpgradeNodeObserver *observer);
    
    void check(float gold, float wood);
    
    // getters
    const UnderWorld::Core::Camp* getCamp() const;
    
protected:
    MapUIUpgradeNode();
    bool init(const UnderWorld::Core::Camp* camp);
    
    // MapUITalentNodeObserver
    virtual void onMapUITalentNodeClickedButton(MapUITalentNode* node) override;
    
private:    
    MapUIUpgradeNodeObserver *_observer;
    const UnderWorld::Core::Camp* _camp;
    std::vector<MapUITalentNode*> _nodes;
};

#endif /* MapUIUpgradeNode_h */
