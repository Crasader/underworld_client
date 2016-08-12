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
#include "BattleDeckCard.h"
#include <unordered_map>

USING_NS_CC;

class DeckData;
class CardSimpleData;
class TabButton;
class UniversalButton;

enum class DeckTabType {
    None,
    All,
    Heroes,
    Soldiers,
    Spells,
};

class BattleDeckLayerObserver
{
public:
    virtual ~BattleDeckLayerObserver() {}
};

class BattleDeckLayer
: public LayerColor
, public BattleDeckCardObserver
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
    
    // BattleDeckCardObserver
    virtual void onBattleDeckCardTouched(BattleDeckCard* pSender) override;
    virtual void onBattleDeckCardUse(BattleDeckCard* pSender) override;
    virtual void onBattleDeckCardInfo(BattleDeckCard* pSender) override;
    
    void createLeftNode();
    void createRightNode();
    BattleDeckCard* createFoundCard(const CardSimpleData* data);
    Node* createUnfoundCard(const std::string& name) const;
    Node* createLine(DeckTabType type) const;
    Node* createUnfoundLine() const;
    void updateCardsCount(int count);
    float getHeight(size_t count, float spaceY) const;
    Point getPosition(int row, int column) const;
    
    // functions
    void initPositions();
    void initCandidates();
    void clearCandidates();
    void saveThisDeck();
    void loadDeck(int idx);
    void setDefaultDeck();
    void setCardType(DeckTabType type);
    std::string getCardTabName(DeckTabType type) const;
    
private:
    class Candidate;
    BattleDeckLayerObserver *_observer;
    
    // UI
    Node* _background;
    std::unordered_map<int, TabButton*> _deckTabButtons;
    std::vector<TabButton*> _cardTabButtons;
    Label* _cardsCountLabel;
    UniversalButton* _sortTypeButton;
    ui::ScrollView* _scrollView;
    Node* _unfoundLine;
    
    std::map<DeckTabType, Candidate*> _candidates;
    std::vector<Node*> _candidateCards;
    
    std::vector<Point> _foundPositions;
    
    // Data
    int _thisDeckIdx;
    DeckData* _thisDeckData;
    std::unordered_map<int, DeckData*> _editedDeckData;
    
    DeckTabType _thisCardType;
};

#endif /* BattleDeckLayer_h */
