//
//  CardInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/6/29.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardInfoLayer_h
#define CardInfoLayer_h

#include "cocos2d.h"

USING_NS_CC;

class CardInfoNode;
class TalentInfoNode;

class CardInfoLayerObserver
{
public:
    virtual ~CardInfoLayerObserver() {}
    virtual void onCardInfoLayerClosed(int card) = 0;
    virtual void onCardInfoLayerUpgradeCard(int card) = 0;
};

class CardInfoLayer : public Layer
{
public:
    static CardInfoLayer* create();
    virtual ~CardInfoLayer();
    void registerObserver(CardInfoLayerObserver *observer);
    void update(int idx);
    
protected:
    CardInfoLayer();
    virtual bool init() override;
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    CardInfoLayerObserver *_observer;
    int _cardId;
    CardInfoNode* _cardInfoNode;
    Label* _titleLabel;
};

#endif /* CardInfoLayer_h */
