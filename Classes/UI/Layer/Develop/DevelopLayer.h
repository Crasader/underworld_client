//
//  DevelopLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DevelopLayer_h
#define DevelopLayer_h

#include "DevelopCard.h"
#include "CardPreview.h"
#include "CardInfoLayer.h"
#include "SpellInfoLayer.h"

USING_NS_CC;

class DevelopLayerObserver
{
public:
    virtual ~DevelopLayerObserver() {}
};

class DevelopLayer
: public LayerColor
, public DevelopCardObserver
, public CardPreviewObserver
, public CardInfoLayerObserver
, public SpellInfoLayerObserver
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
    virtual void onDevelopCardClicked(DevelopCard* pSender, bool canUpgrade) override;
    
    // CardPreviewObserver
    virtual AbstractCard* onCardPreviewCreateCard(int cardId) override;
    virtual void onCardPreviewClickedOpButton(DeckCardOpType type, int cardId) override;
    
    // CardInfoLayerObserver
    virtual void onCardInfoLayerReturn(Node* pSender) override;
    virtual void onCardInfoLayerExit(Node* pSender) override;
    
    // SpellInfoLayerObserver
    virtual void onSpellInfoLayerExit(Node* pSender) override;
    
private:
    DevelopLayerObserver *_observer;
    CardPreview* _cardPreview;
    
    // Data
    DeckManager::FeatureType _featureType;
};

#endif /* DevelopLayer_h */
