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
#include <unordered_map>
#include <unordered_set>

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
    enum class SortType;
    class Candidate;
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
    DeckCard* createFoundCard(const CardSimpleData* data);
    Node* createUnfoundCard(int cid) const;
    Node* createLine(bool isHero) const;
    Node* createUnfoundLine() const;
    void showOpNode(DeckCard* card);
    void hideOpNode();
    void beginEdit(DeckCard* pickedCard);
    void endEdit();
    void updateCardsCount(int count);
    void updateAverageElixir();
    void exchangeCard(DeckCard* from, DeckCard* to);
    void useCard(DeckCard* touchedCard, DeckCard* replaced, const Point& point);
    void useCardCancelled();
    void move(Node* node, const Point& point, const std::function<void()>& callback) const;
    void shake(const std::vector<std::unordered_set<DeckCard*>>& nodes);
    void stopShake();
    void readdChild(Node* parent, Node* child) const;
    DeckCard* getIntersectedCard(const DeckCard* touchedCard) const;
    DeckCard* getIntersectedCard(const DeckCard* touchedCard, const std::unordered_set<DeckCard*>& cards, float& intersectedArea) const;
    Rect getWorldBoundingBox(const Node* node) const;
    float getHeight(size_t count, float spaceY) const;
    Point getPosition(int row, int column) const;
    
    // functions
    void initPositions();
    void initCandidates();
    void clearCandidates();
    void loadCandidates(Candidate* candidates);
    void saveThisDeck();
    void loadDeck(int idx);
    void setDefaultDeck();
    void setNextSortType();
    
#if DECKLAYER_ENABLE_TYPE_FILTER
    void setCardType(DeckTabType type);
    std::string getCardTabName(DeckTabType type) const;
#endif
    std::string getSortTypeName(SortType type) const;
    
private:
    BattleDeckLayerObserver *_observer;
    
    // UI
    Node* _background;
    std::unordered_map<int, TabButton*> _deckTabButtons;
#if DECKLAYER_ENABLE_TYPE_FILTER
    std::vector<TabButton*> _cardTabButtons;
    DeckTabType _thisCardType;
    std::map<DeckTabType, Candidate*> _candidates;
#else
    Candidate* _candidates;
#endif
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
    
    std::unordered_set<DeckCard*> _heroCards;
    std::unordered_set<DeckCard*> _nonheroCards;
    std::unordered_set<DeckCard*> _foundCards;
    std::unordered_set<Node*> _unfoundCards;
    
    std::vector<Point> _foundPositions;
    
    // Data
    int _thisDeckIdx;
    DeckData* _thisDeckData;
    bool _isEditing;
    std::unordered_map<int, DeckData*> _editedDeckData;
    int _thisSortIdx;
};

#endif /* BattleDeckLayer_h */
