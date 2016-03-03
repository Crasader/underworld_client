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
#include "CardNode.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class Card;
}}

class MapUICardDeckObserver
{
public:
    virtual ~MapUICardDeckObserver() {}
    virtual void onMapUICardDeckUnitTouchedBegan(const UnderWorld::Core::Card* card) = 0;
    virtual void onMapUICardDeckUnitTouchedEnded(const UnderWorld::Core::Card* card) = 0;
};

class MapUICardDeck: public Node, public CardNodeObserver
{
public:
    static MapUICardDeck* create();
    virtual ~MapUICardDeck();
    void registerObserver(MapUICardDeckObserver *observer);
    
    void select(const UnderWorld::Core::Card* card);
    void updateTimer(float time);
    void updateResource(float count);
    void insert(const UnderWorld::Core::Card* card);
    void remove(const UnderWorld::Core::Card* card);
    
protected:
    MapUICardDeck();
    virtual bool init() override;
    
    // CardNodeObserver
    virtual void onCardNodeTouchedBegan(CardNode* node) override;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) override;
    
    void createUnitNode(const UnderWorld::Core::Card* card, size_t idx);
    void reload();
    
private:
    MapUICardDeckObserver *_observer;
    Sprite* _candidateSprite;
    Label* _nextLabel;
    Label* _countLabel;
    std::vector<Point> _unitPositions;
    std::vector<CardNode*> _unitNodes;
    std::vector<ProgressTimer*> _resources;
};

#endif /* MapUICardDeck_h */
