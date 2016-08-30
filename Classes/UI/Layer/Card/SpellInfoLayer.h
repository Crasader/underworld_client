//
//  SpellInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SpellInfoLayer_h
#define SpellInfoLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UniversalBoard;
class DevelopCard;
class CardPropertyNode;
class CardSimpleData;

class SpellInfoLayerObserver
{
public:
    virtual ~SpellInfoLayerObserver() {}
    virtual void onSpellInfoLayerExit(Node* pSender) = 0;
    virtual void onSpellInfoLayerUpgrade(Node* pSender, const CardSimpleData* data) = 0;
};

class SpellInfoLayer : public LayerColor
{
public:
    static SpellInfoLayer* create(const CardSimpleData* data);
    virtual ~SpellInfoLayer();
    void registerObserver(SpellInfoLayerObserver *observer);
    
private:
    SpellInfoLayer();
    
    // LayerColor
    bool init(const CardSimpleData* data);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void createNode(Node* node);
    void update(const CardSimpleData* data);
    
private:
    SpellInfoLayerObserver* _observer;
    UniversalBoard* _board;
    DevelopCard* _icon;
    Label* _description;
    std::vector<CardPropertyNode*> _properties;
    const CardSimpleData* _data;
};

#endif /* SpellInfoLayer_h */
