//
//  UnitCardDeck.h
//  Underworld_Client
//
//  Created by Andy on 16/3/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UnitCardDeck_h
#define UnitCardDeck_h

#include "CardDeck.h"

class UnitCardDeck: public CardDeck
{
public:
    static UnitCardDeck* create(int count);
    virtual ~UnitCardDeck();
    
    void updateNextCard(const UnderWorld::Core::Card* card);
    void updateTimer(float time, float duration);
    void updateResource(const std::unordered_map<std::string, float>& resources);
    
protected:
    UnitCardDeck();
    bool init(int count);
    
    void shake();
    void stopShake();
    virtual void addCostHint(int cost) override;
    void removeCostHint();
    void createNextCardNode(const UnderWorld::Core::Card* card);
    void createBlankNextCardNode();
    void createNextCardProgress(Node* parent);
    
private:
    Node* _nextCardNode;
    ProgressTimer* _nextCardProgress;
    Label* _nextLabel;
    Label* _countLabel;
    Node* _costHint;
    const UnderWorld::Core::Card* _nextCard;
    std::vector<std::pair<Node*, ProgressTimer*>> _resources;
    bool _isShaking;
    Point _costStartPosition;
};

#endif /* UnitCardDeck_h */
