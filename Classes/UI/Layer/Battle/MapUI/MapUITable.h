//
//  MapUITable.h
//  Underworld_Client
//
//  Created by Andy on 16/2/27.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUITable_h
#define MapUITable_h

#include "cocos2d.h"
#include "MapUIUnitNode.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class Camp;
}}

class MapUITableObserver
{
public:
    virtual ~MapUITableObserver() {}
    virtual void onMapUITableUnitAdd(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUITableUnitUpgrade(MapUIUnitNode* node) = 0;
    virtual void onMapUITableUnitTouchedBegan(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUITableUnitTouchedEnded(const UnderWorld::Core::Camp* camp) = 0;
};

class MapUITable: public Node, public MapUIUnitNodeObserver
{
public:
    static MapUITable* create(const std::vector<const UnderWorld::Core::Camp*>& camps);
    virtual ~MapUITable();
    void registerObserver(MapUITableObserver *observer);
    
    void reload(float gold, float wood, const UnderWorld::Core::Camp* selectedCamp);
    
protected:
    MapUITable();
    bool init(const std::vector<const UnderWorld::Core::Camp*>& camps);
    
    // MapUIUnitNodeObserver
    virtual void onMapUIUnitNodeClickedAddButton(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUIUnitNodeClickedUpgradeButton(MapUIUnitNode* node) override;
    virtual void onMapUIUnitNodeTouchedBegan(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUIUnitNodeTouchedEnded(const UnderWorld::Core::Camp* camp, bool isValid) override;
    
private:
    MapUITableObserver *_observer;
    std::vector<MapUIUnitNode*> _unitNodes;
};

#endif /* MapUITable_h */
