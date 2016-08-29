//
//  UpgradeCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UpgradeCard_h
#define UpgradeCard_h

#include "AbstractCard.h"

USING_NS_CC;

class DevelopCard;
class UpgradeCard;
class ResourceButton;

class UpgradeCardObserver
{
public:
    virtual ~UpgradeCardObserver() {}
    virtual void onUpgradeCardInfo(UpgradeCard* pSender) = 0;
    virtual void onUpgradeCardUpgrade(UpgradeCard* pSender) = 0;
};

class UpgradeCard : public AbstractCard
{
public:
    static UpgradeCard* create(int cardId);
    virtual ~UpgradeCard();
    void registerObserver(UpgradeCardObserver* observer);
    
    void update(int cardId);
    
    // AbstractCard
    virtual int getCardId() const override;
    
private:
    UpgradeCard();
    bool init(int cardId);
    void updateOthers(int cardId);
    
private:
    UpgradeCardObserver* _observer;
    DevelopCard* _icon;
    bool _touchInvalid;
    Label* _name;
    ResourceButton* _button;
};

#endif /* UpgradeCard_h */
