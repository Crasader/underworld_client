//
//  MapUICardDeck.h
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapUICardDeck_h
#define MapUICardDeck_h

#include "cocos2d.h"
#include "MapUIUnitNode.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class Camp;
}}

class MapUICardDeckObserver
{
public:
    virtual ~MapUICardDeckObserver() {}
    virtual void onMapUICardDeckUnitTouchedBegan(const UnderWorld::Core::Camp* camp) = 0;
    virtual void onMapUICardDeckUnitTouchedEnded(const UnderWorld::Core::Camp* camp) = 0;
};

class MapUICardDeck: public Node, public MapUIUnitNodeObserver
{
public:
    static MapUICardDeck* create(const std::vector<const UnderWorld::Core::Camp*>& camps);
    virtual ~MapUICardDeck();
    void registerObserver(MapUICardDeckObserver *observer);
    
    void select(const UnderWorld::Core::Camp* selectedCamp);
    void updateTimer(float time);
    void updateResource(float count);
    void initial(const std::vector<const UnderWorld::Core::Camp*>& camps);
    void insert(const UnderWorld::Core::Camp* camp);
    void remove(const UnderWorld::Core::Camp* camp);
    
protected:
    MapUICardDeck();
    bool init(const std::vector<const UnderWorld::Core::Camp*>& camps);
    
    // MapUIUnitNodeObserver
    virtual void onMapUIUnitNodeTouchedBegan(const UnderWorld::Core::Camp* camp) override;
    virtual void onMapUIUnitNodeTouchedEnded(const UnderWorld::Core::Camp* camp, bool isValid) override;
    
    void createUnitNode(const UnderWorld::Core::Camp* camp, size_t idx);
    void reload();
    
private:
    MapUICardDeckObserver *_observer;
    Sprite* _candidateSprite;
    Label* _nextLabel;
    Label* _countLabel;
    std::vector<const UnderWorld::Core::Camp*> _camps;
    std::vector<Point> _unitPositions;
    std::vector<MapUIUnitNode*> _unitNodes;
    std::vector<ProgressTimer*> _resources;
};

#endif /* MapUICardDeck_h */
