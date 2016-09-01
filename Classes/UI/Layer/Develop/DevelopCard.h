//
//  DevelopCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DevelopCard_h
#define DevelopCard_h

#include "AbstractCard.h"

USING_NS_CC;

class DeckCard;
class DevelopCard;

class DevelopCardObserver
{
public:
    virtual ~DevelopCardObserver() {}
    virtual void onDevelopCardClicked(DevelopCard* pSender, bool canUpgrade) = 0;
};

class DevelopCard : public AbstractCard
{
public:
    static DevelopCard* create(const AbstractData* data);
    virtual ~DevelopCard();
    void registerObserver(DevelopCardObserver* observer);
    
    void update(const AbstractData* data);
    bool canUpgrade() const;
    
    // AbstractCard
    virtual const AbstractData* getCardData() const override;
    
private:
    DevelopCard();
    bool init(const AbstractData* data);
    
private:
    DevelopCardObserver* _observer;
    DeckCard* _icon;
    bool _touchInvalid;
    ProgressTimer* _pt;
    Label* _amount;
    Sprite* _arrow;
};

#endif /* DevelopCard_h */
