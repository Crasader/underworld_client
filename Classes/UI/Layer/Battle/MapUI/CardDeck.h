//
//  CardDeck.h
//  Underworld_Client
//
//  Created by Andy on 16/4/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardDeck_h
#define CardDeck_h

#include "cocos2d.h"
#include "CardNode.h"

USING_NS_CC;

namespace UnderWorld { namespace Core {
    class Card;
}}

class CardDeck;

class CardDeckObserver
{
public:
    virtual ~CardDeckObserver() {}
    virtual void onCardDeckTouchedBegan(CardDeck* node, const std::string& card, int idx) = 0;
    virtual void onCardDeckTouchedEnded(CardDeck* node, const std::string& card, int idx) = 0;
};

class CardDeck: public Node, public CardNodeObserver
{
public:
    static CardDeck* create(int count);
    virtual ~CardDeck();
    void registerObserver(CardDeckObserver *observer);
    
    std::string getCard(int idx) const;
    void select(int idx);
    void insert(const std::string& name, bool animated);
    void insert(const UnderWorld::Core::Card* card, bool animated);
    void remove(const UnderWorld::Core::Card* card, int index, bool animated);
    void updateCD(int idx, float percentage);
    
protected:
    CardDeck();
    bool init(int count);
    
    CardNode* insert(bool animated);
    void reload();
    virtual void addCostHint(int cost);
    
    // CardNodeObserver
    virtual void onCardNodeTouchedBegan(CardNode* node) override final;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) override final;
    
protected:
    CardDeckObserver *_observer;
    Node* _background;
    std::vector<Point> _positions;
    std::vector<CardNode*> _nodes;
    std::queue<CardNode*> _buffers;
    Point _insertActionStartPosition;
};

#endif /* CardDeck_h */
