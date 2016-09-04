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

class MediumBoard;
class JuniorCard;
class CardPropertyNode;
class AbstractData;
class AbstractProperty;

class SpellInfoLayerObserver
{
public:
    virtual ~SpellInfoLayerObserver() {}
    virtual void onSpellInfoLayerExit(Node* pSender) = 0;
    virtual void onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data) = 0;
};

class SpellInfoLayer : public LayerColor
{
public:
    static SpellInfoLayer* create(int cardId, const AbstractData* data);
    virtual ~SpellInfoLayer();
    void registerObserver(SpellInfoLayerObserver *observer);
    
private:
    SpellInfoLayer();
    
    // LayerColor
    bool init(int cardId, const AbstractData* data);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    void createNode(Node* node);
    void updateProperty(const AbstractProperty* property);
    void updateData(const AbstractData* data);
    
private:
    SpellInfoLayerObserver* _observer;
    MediumBoard* _board;
    JuniorCard* _icon;
    Label* _description;
    std::vector<CardPropertyNode*> _properties;
};

#endif /* SpellInfoLayer_h */
