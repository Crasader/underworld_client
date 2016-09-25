//
//  DevelopLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef DevelopLayer_h
#define DevelopLayer_h

#include "BaseCard.h"
#include "CardPreview.h"

USING_NS_CC;

class DevelopLayerObserver
{
public:
    virtual ~DevelopLayerObserver() {}
};

class DevelopLayer
: public LayerColor
, public BaseCardObserver
, public CardPreviewObserver
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
    
    // BaseCardObserver
    virtual void onBaseCardClicked(BaseCard* pSender) override;
    
    // CardPreviewObserver
    virtual BaseCard* onCardPreviewCreateCard(int cardId) override;
    virtual void onCardPreviewClickedOpButton(CardOpType type, const AbstractData* data) override;
    
private:
    DevelopLayerObserver *_observer;
    CardPreview* _cardPreview;
    
    // Data
    DeckManager::FeatureType _featureType;
};

#endif /* DevelopLayer_h */
