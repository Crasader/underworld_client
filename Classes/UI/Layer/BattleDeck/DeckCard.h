//
//  DeckCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DeckCard_h
#define DeckCard_h

#include "AbstractCard.h"

USING_NS_CC;

class DeckCard;

class DeckCardObserver
{
public:
    virtual ~DeckCardObserver() {}
    virtual void onDeckCardTouched(DeckCard* pSender, ui::Widget::TouchEventType type) = 0;
    virtual void onDeckCardClicked(DeckCard* pSender) = 0;
};

class DeckCard : public AbstractCard
{
public:
    static constexpr float Width = 82;
    static constexpr float Height = 104;
    static DeckCard* create(int cardId);
    virtual ~DeckCard();
    void registerObserver(DeckCardObserver *observer);
    
    void update(int cardId);
    virtual int getCardId() const override;
    
    void shake();
    void stopShake();
    
private:
    DeckCard();
    bool init(int cardId);
    
private:
    DeckCardObserver* _observer;
    Sprite* _icon;
    Node* _costNode;
    Label* _cost;
    int _cardId;
    bool _touchInvalid;
    Point _iconPoint;
};

#endif /* DeckCard_h */
