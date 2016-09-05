//
//  SpellInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SpellInfoLayer_h
#define SpellInfoLayer_h

#include "AbstractInfoLayer.h"

class SpellInfoLayerObserver
{
public:
    virtual ~SpellInfoLayerObserver() {}
    virtual void onSpellInfoLayerExit(Node* pSender) = 0;
    virtual void onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data) = 0;
};

class SpellInfoLayer : public AbstractInfoLayer
{
public:
    static SpellInfoLayer* create(int cardId, const AbstractData* data);
    virtual ~SpellInfoLayer();
    void registerObserver(SpellInfoLayerObserver *observer);
    
private:
    SpellInfoLayer();
    
    void createNode(Node* node);
    virtual void initUI() final;
    virtual void updateProperty(const AbstractProperty* property) final;
    virtual void updateData(const AbstractData* data) final;
    
private:
    SpellInfoLayerObserver* _observer;
};

#endif /* SpellInfoLayer_h */
