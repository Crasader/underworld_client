//
//  BattleDeckLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/8/10.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckLayer_h
#define BattleDeckLayer_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "DeckCard.h"
#include "DeckCardOpNode.h"
#include "DeckEditMask.h"
#include <vector>
#include <unordered_map>

USING_NS_CC;

#define DECKLAYER_ENABLE_TYPE_FILTER    (0)

class DeckData;
class CardSimpleData;
class TabButton;
class UniversalButton;

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
, public DeckCardOpNodeObserver
, public DeckEditMaskObserver
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
    
    // DeckCardOpNodeObserver
    virtual void onDeckCardOpNodeClicked() override;
    virtual void onDeckCardOpNodeClickedButton(DeckCardOpType type, int cardId) override;
    
    // DeckEditMaskObserver
    virtual void onDeckEditMaskTouched(const Point& point) override;
    
    // -------- UI --------
    void createLeftNode();
    void createRightNode();
    DeckCard* createCard(int card);
    Node* createLine(bool isHero) const;
    Node* createUnfoundLine() const;
    void updateCardsCount();
    void updateAverageElixir();
    
    // Oprations Node
    void showOpNode(DeckCard* card);
    void hideOpNode();
    bool setOpNodePosition(DeckCard* card);
    
    // Move cards
    void beginEdit(int cardId);
    void endEdit();
    void onReceivedManagerNotifications(const std::string& notification);
    void sortCards(const std::vector<int>& cards, std::unordered_map<int, DeckCard*>& nodes, const std::vector<Point>& positions);
    void exchangeCard(DeckCard* from, DeckCard* to);
    void exchangeCardCancelled(DeckCard* card);
    void useCard(DeckCard* replaced, bool fromDeck);
    void useCardCancelled();
    
    // Universal Methods
    void move(Node* node, const Point& point, float duration, const std::function<void()>& callback) const;
    void shake(const std::vector<DeckCard*>& nodes) const;
    void stopShake();
    void readdChild(Node* parent, Node* child) const;
    DeckCard* getIntersectedCard(const DeckCard* touchedCard) const;
    DeckCard* getIntersectedCard(const DeckCard* touchedCard, const std::vector<DeckCard*>& cards) const;
    Rect getWorldBoundingBox(const Node* node) const;
    float getHeight(size_t count, float spaceY) const;
    Point getPosition(int row, int column) const;
    
    // Functions
    void loadCandidates();
    void loadDeck(int idx);
    void setNextSortType();
    void resetParams();
    
#if DECKLAYER_ENABLE_TYPE_FILTER
    void setCardType(DeckTabType type);
    std::string getCardTabName(DeckTabType type) const;
#endif
    
private:
    BattleDeckLayerObserver *_observer;
    
    // UI
    Node* _background;
    std::unordered_map<int, TabButton*> _deckTabButtons;
    Label* _cardsCountLabel;
    Label* _averageElixirLabel;
    UniversalButton* _sortTypeButton;
    ui::ScrollView* _scrollView;
    Node* _unfoundLine;
    DeckCardOpNode* _opNode;
    DeckEditMask* _deckEditMask;
    DeckCard* _usedCard;
    Point _usedCardPoint;
    Point _cardOriginalPoint;
    
    std::vector<DeckCard*> _deckCards;
    std::unordered_map<int, DeckCard*> _foundCards;
    std::unordered_map<int, DeckCard*> _unfoundCards;
    
    std::vector<Point> _foundPositions;
    std::vector<Point> _unfoundPositions;
    
    // Data
    bool _isEditing;
    int _thisSortIdx;
};

#endif /* BattleDeckLayer_h */
