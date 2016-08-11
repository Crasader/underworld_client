//
//  BattleDeckCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckCard_h
#define BattleDeckCard_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class BattleDeckCardObserver
{
public:
    virtual ~BattleDeckCardObserver() {}
};

class BattleDeckCard : public ui::Widget
{
public:
    static BattleDeckCard* create(const std::string& name);
    virtual ~BattleDeckCard();
    void registerObserver(BattleDeckCardObserver *observer);
    
    void update(const std::string& name);
    const std::string& getCardName() const;
    
private:
    BattleDeckCard();
    bool init(const std::string& name);
    
private:
    BattleDeckCardObserver* _observer;
    std::string _cardName;
};

#endif /* BattleDeckCard_h */
