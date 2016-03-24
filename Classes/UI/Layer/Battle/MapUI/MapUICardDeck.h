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
    virtual void onMapUICardDeckUnitTouchedBegan(const UnderWorld::Core::Card* card, int idx) = 0;
    virtual void onMapUICardDeckUnitTouchedEnded(const UnderWorld::Core::Card* card, int idx) = 0;
};

class MapUICardDeck: public Node, public CardNodeObserver
{
public:
    static MapUICardDeck* create(int count);
    virtual ~MapUICardDeck();
    void registerObserver(MapUICardDeckObserver *observer);
    
    const UnderWorld::Core::Card* getCard(int idx) const;
    void select(int idx);
    void updateTimer(float time, float duration);
    void updateResource(const std::map<std::string, float>& resources);
    void insert(const UnderWorld::Core::Card* card, bool animated);
    void remove(const UnderWorld::Core::Card* card, bool animated);
    
protected:
    MapUICardDeck();
    bool init(int count);
    
    // CardNodeObserver
    virtual void onCardNodeTouchedBegan(CardNode* node) override;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) override;
    
    void reload();
    void shake();
    void stopShake();
    
private:
    MapUICardDeckObserver *_observer;
    Sprite* _candidateSprite;
    ProgressTimer* _candidateProgress;
    Label* _nextLabel;
    Label* _countLabel;
    std::vector<Point> _unitPositions;
    std::vector<CardNode*> _cardNodes;
    std::vector<std::pair<Sprite*, ProgressTimer*>> _resources;
    std::queue<CardNode*> _buffers;
    bool _isShaking;
    Point _candidateSpritePosition;
};

#endif /* MapUICardDeck_h */
