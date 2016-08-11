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

class UniversalButton;
class BattleDeckCard;

class BattleDeckCardObserver
{
public:
    virtual ~BattleDeckCardObserver() {}
    virtual void onBattleDeckCardUse(BattleDeckCard* pSender) = 0;
    virtual void onBattleDeckCardInfo(BattleDeckCard* pSender) = 0;
};

class BattleDeckCard : public ui::Widget
{
public:
    static constexpr float Width = 82;
    static constexpr float Height = 104;
    static BattleDeckCard* create(const std::string& name);
    virtual ~BattleDeckCard();
    void registerObserver(BattleDeckCardObserver *observer);
    
    void update(const std::string& name);
    void setIsHero(bool isHero);
    void setInDeck(bool inDeck);
    void showHint(bool show);
    const std::string& getCardName() const;
    
private:
    BattleDeckCard();
    bool init(const std::string& name);
    
private:
    BattleDeckCardObserver* _observer;
    Sprite* _icon;
    Node* _costNode;
    Label* _cost;
    ui::Scale9Sprite* _hint;
    UniversalButton* _use;
    UniversalButton* _info;
    std::string _cardName;
    bool _isHero;
    bool _inDeck;
};

#endif /* BattleDeckCard_h */
