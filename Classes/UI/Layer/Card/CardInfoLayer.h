//
//  CardInfoLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/19.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardInfoLayer_h
#define CardInfoLayer_h

#include "RuneCircle.h"
#include "RuneBagLayer.h"
#include "BaseCard.h"
#include "SpellInfoLayer.h"

USING_NS_CC;

class UniversalBoard;
class JuniorCard;
class SeniorCard;
class CardPropertyNode;
class UniversalButton;
class ResourceButton;
class CardData;

class CardInfoLayerObserver
{
public:
    virtual ~CardInfoLayerObserver() {}
    virtual void onCardInfoLayerReturn(Node* pSender) = 0;
    virtual void onCardInfoLayerExit(Node* pSender) = 0;
};

class CardInfoLayer
: public LayerColor
, public RuneCircleObserver
, public RuneBagLayerObserver
, public BaseCardObserver
, public SpellInfoLayerObserver
{
public:
    static CardInfoLayer* create(int cardId, const CardData* data);
    virtual ~CardInfoLayer();
    void registerObserver(CardInfoLayerObserver *observer);
    
private:
    CardInfoLayer();
    
    // LayerColor
    bool init(int cardId, const CardData* data);
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // RuneCircleObserver
    virtual void onRuneCircleClicked(RuneNode* node, int idx) override;
    
    // RuneBagLayerObserver
    virtual void onRuneBagLayerSelected(Node* pSender, RuneNode* node) override;
    
    // BaseCardObserver
    virtual void onBaseCardInfo(BaseCard* pSender) override;
    virtual void onBaseCardUpgrade(BaseCard* pSender) override;
    
    // SpellInfoLayerObserver
    virtual void onSpellInfoLayerExit(Node* pSender) override;
    virtual void onSpellInfoLayerUpgrade(Node* pSender, const AbstractData* data) override;
    
    // -------- UI --------
    void createLeftNode(Node* node);
    void createRightNode(Node* node);
    void createOpButtons(Node* node);
    void onOpButtonClicked(int idx);
    void updateProperty(const AbstractProperty* property);
    void updateData(const CardData* data);
    
private:
    CardInfoLayerObserver* _observer;
    UniversalBoard* _board;
    JuniorCard* _icon;
    Label* _level;
    Label* _profession;
    Label* _description;
    RuneCircle* _runeCircle;
    RuneNode* _selectedRune;
    ResourceButton* _upgradeButton;
    std::vector<CardPropertyNode*> _cardProperties;
    std::vector<Label*> _runeProperties;
    std::vector<SeniorCard*> _skillCards;
    std::vector<UniversalButton*> _opButtons;
    const CardData* _data;
};

#endif /* CardInfoLayer_h */
