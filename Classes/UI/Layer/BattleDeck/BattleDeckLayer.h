//
//  BattleDeckLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckLayer_h
#define BattleDeckLayer_h

#include "DeckCard.h"
#include "DeckEditMask.h"
#include "CardPreview.h"
#include "CardInfoLayer.h"

#define DECKLAYER_ENABLE_TYPE_FILTER    (0)

class UniversalBoard;
class TabButton;

#if DECKLAYER_ENABLE_TYPE_FILTER
enum class DeckTabType {
    None,
    All,
    Heroes,
    Soldiers,
    Spells,
};
#endif

class BattleDeckLayerObserver
{
public:
    virtual ~BattleDeckLayerObserver() {}
};

class BattleDeckLayer
: public LayerColor
, public DeckCardObserver
, public DeckEditMaskObserver
, public CardPreviewObserver
, public CardInfoLayerObserver
{
public:
    static BattleDeckLayer* create();
    virtual ~BattleDeckLayer();
    void registerObserver(BattleDeckLayerObserver *observer);
    
protected:
    BattleDeckLayer();
    
    // LayerColor
    virtual bool init() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // DeckCardObserver
    virtual void onDeckCardTouched(DeckCard* touchedCard, ui::Widget::TouchEventType type) override;
    virtual void onDeckCardClicked(DeckCard* pSender) override;
    
    // DeckEditMaskObserver
    virtual void onDeckEditMaskTouched(const Point& point) override;
    
    // CardPreviewObserver
    virtual AbstractCard* onCardPreviewCreateCard(int cardId) override;
    virtual void onCardPreviewClickedOpButton(DeckCardOpType type, int cardId) override;
    
    // CardInfoLayerObserver
    virtual void onCardInfoLayerReturn(Node* pSender) override;
    virtual void onCardInfoLayerExit(Node* pSender) override;
    
    // -------- UI --------
    void createLeftNode(Node* node);
    void createRightNode(Node* node);
    DeckCard* createCard(int card);
    Node* createLine(bool isHero) const;
    void updateAverageElixir();
    
    // Info
    void showInfo(int cardId);
    
    // Move cards
    void beginEdit(int cardId);
    void endEdit();
    DeckCard* getFoundCard(int cardId) const;
    void exchangeCard(DeckCard* from, DeckCard* to);
    void exchangeCardCancelled(DeckCard* card);
    void useCard(DeckCard* replaced, bool fromDeck);
    void useCardCancelled();
    
    // Universal Methods
    void shake(const std::vector<DeckCard*>& nodes) const;
    void stopShake();
    DeckCard* getIntersectedCard(const DeckCard* touchedCard) const;
    DeckCard* getIntersectedCard(const DeckCard* touchedCard, const std::vector<DeckCard*>& cards) const;
    Rect getWorldBoundingBox(const Node* node) const;
    
    // Functions
    void loadDeck(int idx);
    void resetParams();
    
#if DECKLAYER_ENABLE_TYPE_FILTER
    void setCardType(DeckTabType type);
    std::string getCardTabName(DeckTabType type) const;
#endif
    
private:
    BattleDeckLayerObserver *_observer;
    
    // UI
    UniversalBoard* _background;
    std::unordered_map<int, TabButton*> _deckTabButtons;
    Label* _averageElixirLabel;
    CardPreview* _cardPreview;
    DeckEditMask* _deckEditMask;
    DeckCard* _usedCard;
    Point _usedCardPoint;
    Point _cardOriginalPoint;
    
    std::vector<DeckCard*> _deckCards;
    
    // Data
    DeckManager::FeatureType _featureType;
    bool _isEditing;
};

#endif /* BattleDeckLayer_h */
