//
//  DeckCardInfo.h
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DeckCardInfo_h
#define DeckCardInfo_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "BattleDeckUI.h"

USING_NS_CC;

class DeckCard;

class DeckCardInfoObserver
{
public:
    virtual ~DeckCardInfoObserver() {}
    virtual void onDeckCardInfoClickedButton(DeckCardOpType type, int cardId) = 0;
};

class DeckCardInfo : public LayerColor
{
public:
    static DeckCardInfo* create();
    virtual ~DeckCardInfo();
    void registerObserver(DeckCardInfoObserver *observer);
    
    void setCard(int cardId);
    int getCard() const;
    
private:
    class PropertyNode;
    DeckCardInfo();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    DeckCardInfoObserver* _observer;
    Node* _background;
    DeckCard* _icon;
    Label* _level;
    Label* _profession;
    Label* _description;
    std::vector<PropertyNode*> _properties;
    std::map<DeckCardOpType, UniversalButton*> _buttons;
};

#endif /* DeckCardInfo_h */
