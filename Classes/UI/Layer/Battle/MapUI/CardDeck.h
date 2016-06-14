//
//  CardDeck.h
//  Underworld_Client
//
//  Created by Andy on 16/4/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardDeck_h
#define CardDeck_h

#include "CardNode.h"

extern const float deckCostOffsetX;

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
    void insert(const HMMCard* card, bool animated);
    void remove(const HMMCard* card, int index, bool animated);
    void updateCD(int idx, float percentage);
    void updateResource(const std::unordered_map<std::string, float>& resources);
    
protected:
    CardDeck();
    bool init(int count);
    
    CardNode* insert(bool animated);
    void reload();
    void addCostHint(int cost);
    void removeCostHint();
    
    // CardNodeObserver
    virtual void onCardNodeTouchedBegan(CardNode* node) final;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) final;
    
protected:
    CardDeckObserver *_observer;
    
    // cocos2d
    Node* _background;
    Label* _countLabel;
    std::vector<Point> _positions;
    std::vector<CardNode*> _nodes;
    std::queue<CardNode*> _buffers;
    std::vector<std::pair<Node*, ProgressTimer*>> _resources;
    Node* _costHint;
    Point _insertActionStartPosition;
    Point _costStartPosition;
};

#endif /* CardDeck_h */
