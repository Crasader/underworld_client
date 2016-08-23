//
//  CardInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardInfoLayer_h
#define CardInfoLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "BattleDeckUI.h"

USING_NS_CC;

class DeckCard;

class CardInfoLayerObserver
{
public:
    virtual ~CardInfoLayerObserver() {}
    virtual void onCardInfoLayerClickedButton(DeckCardOpType type, int cardId) = 0;
};

class CardInfoLayer : public LayerColor
{
public:
    static CardInfoLayer* create();
    virtual ~CardInfoLayer();
    void registerObserver(CardInfoLayerObserver *observer);
    
    void setCard(int cardId);
    int getCard() const;
    
private:
    class PropertyNode;
    CardInfoLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    CardInfoLayerObserver* _observer;
    Node* _background;
    DeckCard* _icon;
    Label* _level;
    Label* _profession;
    Label* _description;
    std::vector<PropertyNode*> _properties;
    std::map<DeckCardOpType, UniversalButton*> _buttons;
};

#endif /* CardInfoLayer_h */
