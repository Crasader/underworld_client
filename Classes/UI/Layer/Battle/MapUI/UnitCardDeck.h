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
    
    void updateNextCard(const HMMCard* card);
    void updateTimer(float time, float duration);
    
protected:
    UnitCardDeck();
    bool init(int count);
    
    void shake();
    void stopShake();
    void createNextCardNode(const HMMCard* card);
    void createBlankNextCardNode();
    void createNextCardProgress(Node* parent);
    
private:
    Node* _nextCardNode;
    ProgressTimer* _nextCardProgress;
    Label* _nextLabel;
    const HMMCard* _nextCard;
    bool _isShaking;
};

#endif /* UnitCardDeck_h */
