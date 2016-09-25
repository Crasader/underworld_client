//
//  BattleDeckLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckLayer_h
#define BattleDeckLayer_h

#include "BaseCard.h"
#include "DeckEditMask.h"
#include "CardPreview.h"

#define DECKLAYER_ENABLE_TYPE_FILTER    (0)

class Board;
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
, public BaseCardObserver
, public DeckEditMaskObserver
, public CardPreviewObserver
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
    
    // BaseCardObserver
    virtual void onBaseCardTouched(BaseCard* touchedCard, ui::Widget::TouchEventType type) override;
    virtual void onBaseCardClicked(BaseCard* pSender) override;
    
    // DeckEditMaskObserver
    virtual void onDeckEditMaskTouched(const Point& point) override;
    
    // CardPreviewObserver
    virtual BaseCard* onCardPreviewCreateCard(int cardId) override;
    virtual void onCardPreviewClickedOpButton(CardOpType type, const AbstractData* data) override;
    
    // -------- UI --------
    void createLeftNode(Node* node);
    void createRightNode(Node* node);
    BaseCard* createCard(int card);
    Node* createLine(bool isHero) const;
    void updateAverageElixir();
    
    // Info
    void showInfo(int cardId, const CardData* data);
    
    // Move cards
    void beginEdit(int cardId);
    void endEdit();
    BaseCard* getFoundCard(int cardId) const;
    void exchangeCard(int idxFrom, int idxTo);
    void exchangeCardCancelled(int idx);
    void useCard(int idx, bool fromDeck);
    void useCardCancelled();
    
    // Universal Methods
    bool isIdxValid(int idx) const;
    BaseCard* initBaseCard(const Point& point);
    void moveToDeck(BaseCard* card, int idx);
    void shake(const std::vector<BaseCard*>& nodes) const;
    void stopShake();
    BaseCard* getIntersectedCard(const BaseCard* touchedCard) const;
    BaseCard* getIntersectedCard(const BaseCard* touchedCard, const std::vector<BaseCard*>& cards) const;
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
    Board* _background;
    std::unordered_map<int, TabButton*> _deckTabButtons;
    Label* _averageElixirLabel;
    CardPreview* _cardPreview;
    DeckEditMask* _deckEditMask;
    BaseCard* _usedCard;
    Point _usedCardPoint;
    
    std::vector<BaseCard*> _deckCards;
    std::vector<Point> _deckPositions;
    
    // Data
    DeckManager::FeatureType _featureType;
    bool _isEditing;
};

#endif /* BattleDeckLayer_h */
