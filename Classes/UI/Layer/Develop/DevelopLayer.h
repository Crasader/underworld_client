//
//  DevelopLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DevelopLayer_h
#define DevelopLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "DevelopCard.h"
#include "DeckCardOpNode.h"

USING_NS_CC;

class CardSet;
class UniversalButton;

class DevelopLayerObserver
{
public:
    virtual ~DevelopLayerObserver() {}
};

class DevelopLayer
: public LayerColor
, public DevelopCardObserver
, public DeckCardOpNodeObserver
{
public:
    static DevelopLayer* create();
    virtual ~DevelopLayer();
    void registerObserver(DevelopLayerObserver *observer);
    
protected:
    DevelopLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // DevelopCardObserver
    virtual void onDevelopCardClicked() override;
    
    // DeckCardOpNodeObserver
    virtual void onDeckCardOpNodeClicked() override;
    virtual void onDeckCardOpNodeClickedButton(DeckCardOpType type, int cardId) override;
    
    void createScrollView(Node* parent);
    
private:
    DevelopLayerObserver *_observer;
    Label* _cardsCountLabel;
    UniversalButton* _sortTypeButton;
    ui::ScrollView* _scrollView;
    Node* _unfoundLine;
    DeckCardOpNode* _opNode;
    CardSet* _foundCards;
    CardSet* _unfoundCards;
};

#endif /* DevelopLayer_h */
