//
//  CardInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardInfoLayer_h
#define CardInfoLayer_h

#include "AbstractInfoLayer.h"
#include "RuneCircle.h"
#include "RuneBagLayer.h"
#include "BaseCard.h"
#include "SpellInfoLayer.h"

class SeniorCard;
class XButton;

class CardInfoLayerObserver
{
public:
    virtual ~CardInfoLayerObserver() {}
    virtual void onCardInfoLayerReturn(Node* pSender) = 0;
    virtual void onCardInfoLayerExit(Node* pSender) = 0;
};

class CardInfoLayer
: public AbstractInfoLayer
, public RuneCircleObserver
, public RuneBagLayerObserver
, public BaseCardObserver
, public SpellInfoLayerObserver
{
public:
    static CardInfoLayer* create(int cardId, const AbstractData* data);
    virtual ~CardInfoLayer();
    void registerObserver(CardInfoLayerObserver *observer);
    
private:
    CardInfoLayer();
    
    // RuneCircleObserver
    virtual void onRuneCircleClicked(RuneNode* node, int idx) override;
    
    // RuneBagLayerObserver
    virtual void onRuneBagLayerSelected(Node* pSender, RuneNode* node) override;
    
    // BaseCardObserver
    virtual void onBaseCardInfo(BaseCard* pSender) override;
    virtual void onBaseCardClickedResourceButton(BaseCard* pSender) override;
    
    // SpellInfoLayerObserver
    virtual void onSpellInfoLayerExit(Node* pSender) override;
    virtual void onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data) override;
    
    // -------- UI --------
    void createLeftNode(Node* node);
    void createRightNode(Node* node);
    void createOpButtons(Node* node);
    void onOpButtonClicked(int idx);
    
    // AbstractInfoLayer
    virtual void initUI() final;
    virtual void updateProperty(const DevelopProperty* property) final;
    virtual void updateData(const AbstractData* data) final;
    
private:
    CardInfoLayerObserver* _observer;
    Label* _level;
    Label* _profession;
    RuneCircle* _runeCircle;
    RuneNode* _selectedRune;
    std::vector<Label*> _runeProperties;
    std::vector<SeniorCard*> _skillCards;
    std::vector<XButton*> _opButtons;
};

#endif /* CardInfoLayer_h */
