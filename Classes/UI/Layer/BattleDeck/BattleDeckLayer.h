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
#include "EditDeckMask.h"
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
, public EditDeckMaskObserver
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
    virtual void onDeckCardOpNodeClickedButton(DeckCardOpType type, DeckCard* card) override;
    
    // EditDeckMaskObserver
    virtual void onEditDeckMaskTouched(const Point& point) override;
    
    void createLeftNode();
    void createRightNode();
    DeckCard* createFoundCard(int card);
    Node* createUnfoundCard(int card) const;
    Node* createLine(bool isHero) const;
    Node* createUnfoundLine() const;
    void showOpNode(DeckCard* card);
    void hideOpNode();
    void beginEdit(DeckCard* pickedCard);
    void endEdit();
    void updateCardsCount();
    void updateAverageElixir();
    void onReceivedManagerNotifications(const std::string& notification);
    void exchangeCard(DeckCard* from, DeckCard* to);
    void useCard(DeckCard* touchedCard, DeckCard* replaced, const Point& point);
    void useCardCancelled();
    void move(Node* node, const Point& point, const std::function<void()>& callback) const;
    void shake(const std::vector<std::vector<DeckCard*>>& nodes);
    void stopShake();
    void readdChild(Node* parent, Node* child) const;
    DeckCard* getIntersectedCard(const DeckCard* touchedCard) const;
    DeckCard* getIntersectedCard(const DeckCard* touchedCard, const std::vector<DeckCard*>& cards, float& intersectedArea) const;
    Rect getWorldBoundingBox(const Node* node) const;
    float getHeight(size_t count, float spaceY) const;
    Point getPosition(int row, int column) const;
    
    // functions
    bool find(const std::vector<DeckCard*>& cards, const DeckCard* data) const;
    bool replace(std::vector<DeckCard*>& cards, DeckCard* used, DeckCard* replaced) const;
    void exchange(std::vector<DeckCard*>& cards, DeckCard* from, DeckCard* to) const;
    void loadCandidates();
    void loadDeck(int idx);
    void setNextSortType();
    
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
    EditDeckMask* _editDeckMask;
    Point _cardOriginalPoint;
    DeckCard* _usedCard;
    Point _usedCardPoint; // TODO: remove the parameter
    
    std::vector<DeckCard*> _deckCards;
    std::vector<DeckCard*> _foundCards;
    std::vector<Node*> _unfoundCards;
    
    std::vector<Point> _foundPositions;
    
    // Data
    int _thisDeckIdx;
    bool _isEditing;
    int _thisSortIdx;
};

#endif /* BattleDeckLayer_h */
