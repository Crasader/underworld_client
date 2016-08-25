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

USING_NS_CC;

class DevelopCard;
class CardSimpleData;

class CardInfoLayerObserver
{
public:
    virtual ~CardInfoLayerObserver() {}
    virtual void onCardInfoLayerReturn(Node* pSender) = 0;
    virtual void onCardInfoLayerExit(Node* pSender) = 0;
};

class CardInfoLayer : public LayerColor
{
public:
    static CardInfoLayer* create(int cardId);
    virtual ~CardInfoLayer();
    void registerObserver(CardInfoLayerObserver *observer);
    
    int getCard() const;
    
private:
    class PropertyNode;
    CardInfoLayer();
    
    // LayerColor
    bool init(int cardId);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
private:
    CardInfoLayerObserver* _observer;
    DevelopCard* _icon;
    Label* _level;
    Label* _profession;
    Label* _description;
    std::vector<PropertyNode*> _properties;
    const CardSimpleData* _data;
};

#endif /* CardInfoLayer_h */
